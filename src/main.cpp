/*!
  Ponko avatar with WxBeacon2
  Receive data from WxBeacon2 (https://jp.weathernews.com/)
  [OEM OMRON 2JCIE-BL01]

  @file   main.cpp
  @brief  Program entry
*/
#include <M5Unified.h>
#include <M5GFX.h>
#include <esp_system.h>
#include <esp_bt.h> // esp_bt_controller_mem_release
#include <esp_random.h> // hardware RNG
#include <WiFi.h>

#include "utility.hpp"
#include "aq_talk.hpp"
#include "ponko_avatar.hpp"
#include "wni_ticker.hpp"
#include "weather_map.hpp"
#include "progress.hpp"
#include "wb2/wxbeacon2_task.hpp"
#include "wb2/wxbeacon2_log.hpp"
#include "jma/jma_task.hpp"
#include <gob_datetime.hpp>
#include "cpu_usage.hpp"

#ifdef ARDUINO_M5STACK_Core2
#include "himawari/himawari.hpp"
#include "himawari/himawari_task.hpp"
#include "himawari/himawari_screen.hpp"
#endif

#include <ctime>
#include <cstdio>
#include <algorithm>
#include <string>
#include <map>

#if __has_include (<esp_idf_version.h>)
#include <esp_idf_version.h>
#else // esp_idf_version.h has been introduced in Arduino 1.0.5 (ESP-IDF3.3)
#define ESP_IDF_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))
#define ESP_IDF_VERSION ESP_IDF_VERSION_VAL(3,2,0)
#endif

using goblib::datetime::LocalTime;
using goblib::datetime::LocalDateTime;
using goblib::datetime::OffsetDateTime;

namespace
{
// Automatic request interval.
constexpr uint16_t AUTO_REQUEST_INTERVAL_SEC = 1 * 60;

// NTP serve URL
const char* ntpURL[] =
{
    "ntp.nict.jp",
    "ntp.jst.mfeed.ad.jp",
    "time.cloudflare.com",
};

// For configurate time
#ifndef M5S_WXBEACON2_TIMEZONE_LOCATION
#define M5S_WXBEACON2_TIMEZONE_LOCATION "Asia/Tokyo"
#endif

Avatar* avatar;
Ticker* ticker;
WeatherMap* weatherMap;
#ifdef ARDUINO_M5STACK_Core2
HimawariScreen* himawariScreen;
#endif
ProgressIcon* progress;
bool forceRender = true; // Force rendering all
time_t lastUpdate = -1;
time_t voiceEnd = -1;

// For task settings. (piority low:0) [Beware of WDT]
constexpr UBaseType_t advertisePriority = 1;
constexpr BaseType_t advertiseCore = 0; // NimBLE task must be core 0. see also https://gitter.im/NimBLE-Arduino/community?source=orgpage

constexpr UBaseType_t forecastPriority = 1;
constexpr BaseType_t forecastCore = 0;

constexpr UBaseType_t himawariPriority = 1;
constexpr BaseType_t himawariCore = 0;

constexpr UBaseType_t aqtalkPriority = 1;
constexpr BaseType_t aqtalkCore = 1;

constexpr UBaseType_t speakerPriority = 1;
constexpr BaseType_t speakerCore = 0;

// 
bool existsBeacon = false;
WxBeacon2::AdvertiseData advertiseData;
bool updatedAdvertise = false;

//
struct Weather
{
    jma::officecode_t oc;
    jma::weathercode_t wc;
    int8_t lowTemp, highTemp;

    String toString() const
    {
        char buf[128];
        // low == temp means Iinvalid data.(The lowest highest temperature on the day of the announcement is invalid)
        if(lowTemp != highTemp)
        {
            snprintf(buf, sizeof(buf), "[%s] %s %d℃/%d℃",
                     jma::officesCodeToString(oc),
                     jma::weatherCodeToString(wc),
                     highTemp, lowTemp);
        }
        else
        {
            snprintf(buf, sizeof(buf), "[%s] %s",
                     jma::officesCodeToString(oc),
                     jma::weatherCodeToString(wc));
        }
        buf[sizeof(buf)-1] = '\0';
        return String(buf);
    }

};
std::map<OffsetDateTime, std::vector<Weather> > forecast;
OffsetDateTime requestForecastDatetime;
bool updatedForecast = false;

//
#ifdef ARDUINO_M5STACK_Core2
const uint8_t* himawariImage = nullptr;
himawari::Band himawariBand;
OffsetDateTime himawariDatetime;
bool updatedHimawari = false;
#endif

 

// Ticker text
PROGMEM const char DEFAULT_TICKER_TEXT[] = "WEATHEROID Type A Airi    ";
PROGMEM const char NOTICE_TICKER_TEXT[] = "Press and hold C to put the WxBeacon2 into the broadcast mode.    ";
PROGMEM const char NOT_EXISTS_BEACON_TICKER_TEXT[] = "WxBeacon2 IS NOT EXISTS.    ";
PROGMEM const char REQUEST_TICKER_TITLE[] = "Request...";
PROGMEM const char REQUEST_ADV_TICKER_TEXT[] = "Waiting receive Advertise.    ";
PROGMEM const char REQUEST_FORECAST_TICKER_TEXT[] = "Waiting receive forecast.    ";

// Get std::string that formatted with args.
#if 0
template<typename ...Args> std::string formatString(const char* fmt, Args... args)
{
    size_t sz = snprintf(nullptr, 0U, fmt, args...); // calculate length
    char buf[sz + 1];
    snprintf(buf, sizeof(buf), fmt, args...);
    return std::string(buf, buf + sz);
}
#else
std::string formatString(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    size_t sz = vsnprintf(nullptr, 0U, fmt, args); // calculate length
    char buf[sz + 1];
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return std::string(buf, buf + sz);
}
#endif

// Configurate Time by NTP.
void configTime()
{
    // WiFi connect
    WiFi.begin(); // Connect to credential in Hardware. (ESP32 saves the last Wifi connection)
    int tcount = 20;
    while(tcount-- > 0 && WiFi.status() != WL_CONNECTED)
    {
        M5.Display.printf(".");
        delay(500);
    }
    if(WiFi.status() != WL_CONNECTED)
    {
        WB2_LOGE("Failed to connect WiFi");
        abort();
    }

    auto ptz = goblib::datetime::locationToPOSIX(M5S_WXBEACON2_TIMEZONE_LOCATION);
    WB2_LOGI("tz:[%s]", ptz ? ptz : "NONE");
    configTzTime(ptz ? ptz : "", ntpURL[0], ntpURL[1], ntpURL[2]);
    // waiting for time synchronization
    {
        std::tm discard{};
        getLocalTime(&discard, 5 * 1000);
    }
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

//
bool canRequest()
{
#ifdef ARDUINO_M5STACK_Core2
    return !himawari::busy() && !updatedHimawari &&
#else
    return 
#endif
            !busyAdvertise() && !updatedAdvertise &&
            !jma::busyForecast() && !updatedForecast;
}

// --------------------------------
// Request advertise
void _requestAdvertise()
{
    if(canRequest())
    {
        requestAdvertise();
        ticker->setTitle(REQUEST_TICKER_TITLE);
    }
    else
    {
        WB2_LOGI("Busy");
    }
}

// Callback on get advertise.
void callbackOnAdvertise(const bool exists, const WxBeacon2::AdvertiseData& ad)
{
    existsBeacon = exists;
    updatedAdvertise = true;
    advertiseData = ad;
}

// Play advertise
void playAdvertiseData(const WxBeacon2::AdvertiseData& data)
{
    forceRender = true;
    avatar->closeup();
    weatherMap->hide();
#ifdef ARDUINO_M5STACK_Core2
    himawariScreen->hide();
#endif    
    ticker->setTitle("WxBeacon2");
    aq_talk::stopAquesTalk();

    auto de = data.getE();
    if(!existsBeacon || data.format() != WxBeacon2::ADVFormat::E || !de)
    {
        ticker->setText(existsBeacon ? NOTICE_TICKER_TEXT : NOT_EXISTS_BEACON_TICKER_TEXT);
        ticker->setColor(Ticker::Color::Purple);
        return;
    }

    auto vs = formatString("kion <NUMK VAL=%3.1f>do."
                           "situdo <NUMK VAL=%3.1f>pa-sento."
                          "akarusa <NUMK VAL=%d>rukkusu."
                          "yu-bui <NUMK VAL=%3.1f>."
                          "kiatu <NUMK VAL=%4.1f>hekutopasukaru."
                          "souon <NUMK VAL=%3.1f>desiberu."
                          "fukaisisu- <NUMK VAL=%3.1f>."
                          "neccyu-syoukikendo <NUMK VAL=%3.1f>"
                          "desu."
                           ,
                          (float)de->temperature(),
                          (float)de->relativeHumidity(),
                          (int)de->ambientLight(),
                          (float)de->uvIndex(),
                          (float)de->presure(),
                          (float)de->soundNoise(),
                          (float)de->discomfortIndex(),
                          (float)de->heatstroke()
                          );
    aq_talk::playAquesTalk(vs.c_str(), 120);

    //    auto ts = formatString("TEMP %3.1fdegC HUM %3.1f%% AL %dlx UV %3.1f PRESURE %4.1fhPa N %3.1fdB DCMFT %3.1f WBGT %3.1f   ",
    auto ts = formatString("気温:%3.1f度 湿度:%3.1f%% 明度:%dlx UV指数:%3.1f 気圧:%4.1fhPa 騒音:%3.1fdB 不快指数:%3.1f WBGT:%3.1f        ",
                           (float)de->temperature(),
                           (float)de->relativeHumidity(),
                           (int)de->ambientLight(),
                           (float)de->uvIndex(),
                           (float)de->presure(),
                           (float)de->soundNoise(),
                           (float)de->discomfortIndex(),
                           (float)de->heatstroke()
                           );
    ticker->setLevelWBGT(de->heatstroke()); // Set telop color.
    ticker->setText(ts.c_str());
}

// --------------------------------
// Request forecast
void _requestForecast()
{
    if(canRequest())
    {
#if 0
        // TEST
        forecast.clear();
        OffsetDateTime odt = OffsetDateTime::now();
        requestForecastDatetime = odt;
        const jma::officecode_t requestTable[] =
                {
                    16000, // Sapporo
                    40000, // Sendai
                    130000, // Tokyo
                    150000, // Niigata
                    170000, // Kanazawa
                    230000, // Nagoya
                    270000, // Oosaka
                    340000, // Hiroshima
                    390000, // Kochi
                    400000, // Fukuoka
                    471000, // Okinawa
                };

        auto& wv = forecast[odt];
        jma::weathercode_t wc = 200;
        for(auto& e : requestTable)
        {
            wv.push_back({e, wc++, -12, 38});
        }
        updatedForecast = true;
        return;
#endif

        forecast.clear();
        auto odt = OffsetDateTime::now();
        if(odt.toLocalTime() > LocalTime(17, 0, 0))
        {
            auto ldt = odt.toLocalDateTime();
            auto epoch = ldt.toEpochSecond(odt.offset()) + 86400; // next day
            ldt = LocalDateTime::ofEpochSecond(epoch, odt.offset());
            odt = OffsetDateTime::of(ldt, odt.offset());
        }
        requestForecastDatetime = odt;
        WB2_LOGI("request forecast:%s", odt.toString().c_str());

        jma::requestForecast();
        progress->initRatio();
        progress->showProgress();
        
        ticker->setTitle(REQUEST_TICKER_TITLE);
    }
    else
    {
        WB2_LOGI("Busy");
    }
}

// Callback on progess
void callbackOnProgressForecast(const size_t readed, const size_t size)
{
    float ratio = (float)readed / size;
    progress->setRatio(ratio);
}

// Callback on get forecast.
void callbackOnForecast(const jma::officecode_t oc, const jma::Forecast& fc, const jma::WeeklyForecast& wfc)
{
    if(oc == 0)
    {
        progress->hideProgress();
        updatedForecast = true;
        return;
    }
        
    if(fc.existsTopWeatherCodes() && fc.existsTopTemp())
    {
        auto& wcodes = fc.TopWeatherCodes();
        auto& timedef = fc.TopWeatherTime();
        auto& temps = fc.TopTemp();
        auto& timedefTemps = fc.TopTempTime();
        assert(wcodes.size() == timedef.size());
        assert(temps.size() == timedefTemps.size());
        
        WB2_LOGV("wcodes:%zu temps:%zu", wcodes.size(), temps.size());
        
        for(uint8_t i = 0; i < wcodes.size(); ++i)
        {
            Weather weather { oc, wcodes[i], 0, 0 };

            auto odt = timedef[i];
            auto& v = forecast[odt]; // create if empty.

            auto it = std::find_if(timedefTemps.begin(), timedefTemps.end(),
                                   [&odt](const OffsetDateTime& odtTemps)
                                   {
                                       return odt.toLocalDate() == odtTemps.toLocalDate();
                                   });
            auto idx = std::distance(timedefTemps.begin(), it);
            // Store low/high temps if exists.
            if(idx < timedefTemps.size())
            {
                weather.lowTemp = temps[idx];
                weather.highTemp = temps[idx+1];
            }
            v.push_back(weather);
            WB2_LOGV("%s:%u:%u %d/%d", odt.toString().c_str(), weather.oc, weather.wc, weather.lowTemp, weather.highTemp);
        }
    }
    else
    {
        WB2_LOGD("Failed %d/%d",fc.existsTopWeatherCodes(),fc.existsTopTemp());
    }
}

// Play forecast
void playForecast()
{
    forceRender = true;
#ifdef ARDUINO_M5STACK_Core2
    himawariScreen->hide();
#endif
    avatar->wipe(72, 32, 0.30f);
    weatherMap->setDatetime(requestForecastDatetime);
    
    if(forecast.empty())
    {
        avatar->closeup();
        weatherMap->hide();
        ticker->setTitle("ERROR");
        ticker->setColor(Ticker::Color::Purple);
        ticker->setText(DEFAULT_TICKER_TEXT);
        return;
    }

    String ts;
    ts.reserve(256);
    auto vs = formatString("<NUMK VAL=%d COUNTER=gatu><NUMK VAL=%d COUNTER=nichi>no/tenki'o/osirase'simasu  ",
                           requestForecastDatetime.month(), requestForecastDatetime.day());

    weatherMap->clearIcon();
    for(auto& e : forecast) // each date-time
    {
        WB2_LOGV("%s:[%s] %zu", requestForecastDatetime.toString().c_str(), e.first.toString().c_str(), e.second.size());

        if(e.first.toLocalDate() != requestForecastDatetime.toLocalDate()) { continue; } // Skip data.

        auto wv = e.second;
        std::sort(wv.begin(), wv.end(), [](const Weather& a, const Weather& b) { return a.oc < b.oc; }); // Ascend officecode_t
        for(auto& w : wv)
        {
            ts += w.toString() + ' ';
            vs += formatString("%s %s  #", aq_talk::officeCodeToTalk(w.oc), aq_talk::weatherCodeToTalk(w.wc));

            weatherMap->addIcon(w.oc, w.wc);
        }
        ts += "        ";
    }
    aq_talk::playAquesTalk(vs.c_str(), 120);

    ticker->setTitle("Weather");
    ticker->setText(ts.c_str());
    ticker->setColor(Ticker::Color::Green);
    weatherMap->show();
}

// --------------------------------
// Callback on End of aqtalk.
void callbackOnEndAqTalk()
{
    std::time(&voiceEnd);
}

// --------------------------------
// Request himawari image
void _requestHimawari()
{
#ifdef ARDUINO_M5STACK_Core2
    if(canRequest())
    {
#if 0
        himawari::request(OffsetDateTime::now(),
                          (himawari::Area)(esp_random() % gob::to_underlying(himawari::Area::Max)),
                          (himawari::Band)(esp_random() % gob::to_underlying(himawari::Band::Max))
                          );
#else
        himawari::request(OffsetDateTime::now(),
                          himawari::Area::Japan,
                          (himawari::Band)(esp_random() % gob::to_underlying(himawari::Band::Max))
                          );
#endif
        progress->initRatio();
        progress->showProgress();
        progress->setRatio(0.0f);
        ticker->setTitle(REQUEST_TICKER_TITLE);
    }
    else
    {
        WB2_LOGD("busy");
    }
#endif
}

#ifdef ARDUINO_M5STACK_Core2
// Callback on progress
void callbackOnProgressHimawari(const size_t readed, const size_t size)
{
    float ratio = (float)readed / size;
    progress->setRatio(ratio);
}

// Callback on get himawwri image.
void callbackOnHimawariImage(const uint8_t* ptr, const himawari::Band band, const OffsetDateTime& odt)
{
    progress->hideProgress();
    himawariImage = ptr;
    himawariBand = band;
    himawariDatetime = odt;
    updatedHimawari = true;
}

PROGMEM static const char talkHimawari[] = "kisyou'eisei'himawarino 'gazouo 'gorannkudasai";

// Draw himawari image
void drawHimawari()
{
    forceRender = true;
    weatherMap->hide();
    himawariScreen->set(himawariDatetime, himawariImage);

    if(!himawariImage)
    {
        himawariScreen->hide();
        avatar->closeup();
        ticker->setTitle("ERROR");
        ticker->setColor(Ticker::Color::Purple);
        ticker->setText(DEFAULT_TICKER_TEXT);
        return;
    }
    aq_talk::playAquesTalk(talkHimawari, 120);
    himawariScreen->show();
    avatar->wipe(204, 152 , 0.25f);
    ticker->setTitle("Satellite");
    String ts = himawari::bandToString(himawariBand) + "    ";
    ticker->setText(ts.c_str());
    ticker->setColor(Ticker::Color::Green);
}
#endif

PROGMEM static const char t0[] = "minnasa--n,ponnbanwa,weza-roido'taipuei,airi'desu";
PROGMEM static const char t1[] = "konsyu-no/saikaiwa,'kaniza";
PROGMEM static const char t2[] = "weza-roido'una'na-i wa arimasen";
const char* talkTable[] = { t0, t1, t2 };
void talkRandom()
{
    auto idx = esp_random() % gob::size(talkTable);
    aq_talk::playAquesTalk(talkTable[idx], 110);
}
//
}

// Setup
void setup()
{
    WB2_LOGI("Top of setup free:%u internal:%u", esp_get_free_heap_size(), esp_get_free_internal_heap_size());
    WB2_LOGI("PSRAM:size:%u free:%u", ESP.getPsramSize(), ESP.getFreePsram());
    
    // log level
    esp_log_level_set("wifi", (esp_log_level_t)0);
    esp_log_level_set("NimBLE", (esp_log_level_t)0);
    esp_log_level_set("NimBLEScan", (esp_log_level_t)0);
    esp_log_level_set("NimBLEDevice", (esp_log_level_t)0);
    esp_log_level_set("NIMBLE_NVS", (esp_log_level_t)0);

    // Incrase internal heap.
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    
    //
    M5.begin();  
    auto bd = M5.getBoard();
    WB2_LOGI("M5.board : %x", bd); // 2:M5Stack Basic/Gray 3:Core2
    if (M5.Display.width() < M5.Display.height())
    {
        M5.Display.setRotation(M5.Display.getRotation() ^ 1);
    }
    auto scfg = M5.Speaker.config();
    scfg.task_priority = speakerPriority;
    scfg.task_pinned_core = speakerCore;
    M5.Speaker.config(scfg);
    M5.Speaker.begin();
#ifdef NDEBUG
    M5.Speaker.setVolume(bd == m5::board_t::board_M5Stack ? 128 : 64);
#else
    M5.Speaker.setVolume(bd == m5::board_t::board_M5Stack ? 80 : 40);
#endif
    
    delay(500);
    M5.Display.setBrightness(40);
    M5.Display.clear();

    // Avatar,Ticker and WeatherMap
    avatar = new Avatar();
    assert(avatar);
    ticker = new Ticker();
    assert(ticker);
    ticker->setText(DEFAULT_TICKER_TEXT);
    weatherMap = new WeatherMap();
    assert(weatherMap);
#ifdef ARDUINO_M5STACK_Core2
    himawariScreen = new HimawariScreen();
    assert(himawariScreen);
#endif
    progress = new ProgressIcon();
    assert(progress);

    // ConfigTime
    configTime();
    M5.Display.fillScreen(TFT_DARKGREEN);

    // WxBeacon2 / Forecast / Himawari
    initilizeAdvertise(advertisePriority, advertiseCore, callbackOnAdvertise);
    jma::initializeForecast(forecastPriority, forecastCore, callbackOnForecast, callbackOnProgressForecast);
#ifdef ARDUINO_M5STACK_Core2
    himawari::initialize(himawariPriority, himawariCore, callbackOnHimawariImage, callbackOnProgressHimawari);
#endif
    
    // AquesTalk
    auto ret = aq_talk::initialize(aqtalkPriority, aqtalkCore, callbackOnEndAqTalk);
    WB2_LOGD("aq_talk::setup : %d", ret);

    // Get advertise from WxBeacon2.
    _requestAdvertise();

    //
    {
        goblib::datetime::LocalDateTime ldt = goblib::datetime::LocalDateTime::now();
        WB2_LOGI("Configurated time : %s (%s)", ldt.toString().c_str(), M5S_WXBEACON2_TIMEZONE_LOCATION);
        WB2_LOGI("End of setup free:%u internal:%u large:%u large internal:%u",
                 esp_get_free_heap_size(), esp_get_free_internal_heap_size(),
                 heap_caps_get_largest_free_block(MALLOC_CAP_DMA),
                 heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
        WB2_LOGI("PSRAM:size:%u free:%u", ESP.getPsramSize(), ESP.getFreePsram());
        WB2_LOGI("ESP-IDF Version %d.%d.%d",
                 (ESP_IDF_VERSION>>16) & 0xFF, (ESP_IDF_VERSION>>8)&0xFF, ESP_IDF_VERSION & 0xFF);
    }
    M5.Display.startWrite(); // Occupy DMA Bus

    cpu_usage::initialize();
}

namespace
{
bool longPressA{false}, longPressB{false},longPressC{false};
}

// Main loop
void loop()
{
    auto start = millis();
    M5.update();

    // WARNING : If the beacon mode is changed, all data stored in WxBeacon2 will be lost.
    // WARNING : After the mode change, Abort system.
    /*
      C button
      long press : Set beacon mode to GeneralBroadcaster2 for this application.
      press : show/hide ticker
    */
    if(!longPressC && M5.BtnC.pressedFor(1000) && canRequest())
    {
        longPressC = true;
        WB2_LOGI("Set beacon mode to broadcast mode");
        auto b = changeBeaconMode();
        WB2_LOGI("Result : %d", b);
        abort();
    }
    if(M5.BtnC.wasReleased())
    {
        if(!longPressC)
        {
            ticker->show(!ticker->isShow());
            forceRender = true;
        }
        longPressC = false;
    }

    /*
      B button
      long press : Set default setting for WNI aplication.
      press : (none)
    */
    if(!longPressB && M5.BtnB.pressedFor(1000) && canRequest())
    {
        longPressB = true;
        WB2_LOGI("Set default settings");
        aq_talk::stopAquesTalk();
        auto b = changeDefaultSetting();
        WB2_LOGI("Result : %d", b);
        abort();
    }
    if(M5.BtnB.wasReleased())
    {
        if(!longPressB)
        {
            // nop
        }
        longPressB = false;
    }

    /*
      A button
      long press : Obtain forecast force.
      press : Obtain beacon datat force.
    */
    if(!longPressA && M5.BtnA.pressedFor(1000))
    {
        longPressA = true;
        _requestForecast();
        //_requestHimawari();
    }
    if(M5.BtnA.wasReleased())
    {
        if(!longPressA)
        {
            _requestAdvertise();
        }
        longPressA = false;
    }

    time_t now{};
    std::time(&now);
  
    // Play latest advertise data if exists.
    if(updatedAdvertise)
    {
        std::time(&lastUpdate);
        updatedAdvertise = false;
        playAdvertiseData(advertiseData);
    }

    // Play latest forecast
    if(updatedForecast)
    {
        std::time(&lastUpdate);
        updatedForecast = false;
        playForecast();
    }

    // Draw himawari image
#ifdef ARDUINO_M5STACK_Core2
    if(updatedHimawari)
    {
        std::time(&lastUpdate);
        updatedHimawari = false;
        drawHimawari();
    }
#endif

    // Random talking
    if(!aq_talk::busy() && voiceEnd > 0 && std::difftime(now, voiceEnd) >= 60)
    {
        talkRandom();
    }
    
    // Update
    avatar->pump();
    ticker->pump();
#ifdef ARDUINO_M5STACK_Core2
    progress->pump(busyAdvertise(), (jma::busyForecast() || himawari::busy()));
#else
    progress->pump(busyAdvertise(), jma::busyForecast());
#endif

    // Rendering
    {
        weatherMap->render(&M5.Display, forceRender);
#ifdef ARDUINO_M5STACK_Core2
        himawariScreen->render(&M5.Display, forceRender);
#endif
        avatar->render(&M5.Display, forceRender);
        ticker->render(&M5.Display);
        progress->render(&M5.Display);
        forceRender = false;

#if !defined(NDEBUG) && defined(M5S_WXBEACON2_DEBUG_INFO)
        M5.Display.setCursor(128, 120);
# ifdef ARDUINO_M5STACK_Core2
        M5.Display.printf("a:%d f:%d h:%d aq:%d s:%d",
                          busyAdvertise(), jma::busyForecast(), himawari::busy(),
                          aq_talk::busy(), M5.Speaker.isPlaying());
# else
        M5.Display.printf("a:%d f:%d aq:%d s:%d",
                          busyAdvertise(), jma::busyForecast(), aq_talk::busy(), M5.Speaker.isPlaying());
# endif
        M5.Display.setCursor(128, 128);
        M5.Display.printf("ih:%06u ilf:%06u",
                          esp_get_free_internal_heap_size(),
                          heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
        M5.Display.setCursor(128, 136);
        M5.Display.printf("c0:%03.1f c1:%03.1f", cpu_usage::cpu0(), cpu_usage::cpu1());
        cpu_usage::reset();
#endif
    }


#if 1
    // Auto request
    if(canRequest() && lastUpdate > 0 && std::difftime(now, lastUpdate) >= AUTO_REQUEST_INTERVAL_SEC)
    {
        //_requestAdvertise();
        _requestHimawari();
    }
#endif

    // Keep about 30 FPS.
    auto end = millis();
#ifdef M5S_WXBEACON2_DEBUG_INFO
    if((end - start) > (1000/30))
    {
        WB2_LOGD("over:%lu", (end - start) - (1000/30));
    }
#endif
    delay((end - start) >= (1000/30) ? 1 : (1000/30) - (end -start));
}
