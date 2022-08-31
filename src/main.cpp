/*!
  Ponko avatar with WxBeacon2
  Receive data from WxBeacon2 (https://jp.weathernews.com/)
  [OEM OMRON 2JCIE-BL01]

  @file   main.cpp
  @brief  Program entry
*/
#include <M5Unified.h>
#include <M5GFX.h>

#include <esp_log.h>
#include <esp_system.h>
#include <WiFi.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>

#include "wxbeacon2_ble.hpp"
#include "wxbeacon2_log.hpp"
#include "aq_talk.hpp"
#include "ponko_avatar.hpp"
#include "wni_ticker.hpp"

#include <ctime>
#include <cstdio>

// Settings
namespace
{
// Auto talking interval time (Unit:second)
constexpr uint16_t TALKING_INTERVAL_SEC = 60 * 3;
// Measurement interval time (Unit:second) for broadcast mode
constexpr uint16_t MEASUREMENT_INTERVAL_BROADCAST_SEC = 60 * 3;
// turnon LED when got advertise data.
constexpr bool TURNON_LED = false;
}

namespace
{
// Get std::string that formatted with args.
#if 0
template<typename ...Args> std::string formatString(const char* fmt, Args... args)
{
    size_t sz = snprintf(nullptr, 0U, fmt, args...); // calculate length
    char buf[sz + 1];
    snprintf(buf, sizeof(buf), fmt, args...);
    return std::string(buf, buf + sz + 1);
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
    return std::string(buf, buf + sz + 1);
}
#endif

// NTP serve URL
const char* ntpURL[] =
{
    "ntp.nict.jp",
    "ntp.jst.mfeed.ad.jp",
    "time.cloudflare.com",
};

// For configurate time
#ifdef M5S_WXBEACON2_GMT_OFFSET_HOUR
long gmtOffsetSec = 3600 * M5S_WXBEACON2_GMT_OFFSET_HOUR;
#else
long gmtOffsetSec = 3600 * 9; // JST(GMT+9) as default.
#endif

#ifdef M5S_WXBEACON2_DAYLIGHT_OFFSET_SEC
long daylightOffsetSec = M5S_WXBEACON2_DAYLIGHT_OFFSET_SEC;
#else
long daylightOffsetSec = 0; // 0 as default
#endif

#ifdef M5S_WXBEACON2_WIFI_HOST_NAME
#define G_STRINGFY_(s) #s
#define G_STRINGFY(s) G_STRINGFY_(s)
const char* WIFI_HOST_NAME = G_STRINGFY(M5S_WXBEACON2_WIFI_HOST_NAME);
#else
const char* WIFI_HOST_NAME = nullptr;
#endif

// Client
WxBeacon2Client* wb2Client = nullptr;
NimBLEAddress wb2address; // detected wxbeacon2 address

/*
  WxBeacon2Client with scoped
  Call disconnect on destruct.
*/
struct ScopedWB2Client
{
    ScopedWB2Client(WxBeacon2Client& client) : _client(client) {}
    ~ScopedWB2Client() { _client.disconnect(); }
    WxBeacon2Client& client() { return _client; }
  private:    
    WxBeacon2Client& _client;
};

/* 
  Change broadcast mode
  @warning  Changing to broadcast mode will not work with the WNI application.
  @warning If you want to link with the WNI application, call the changeDefaultSetting function.
  @warning If the beacon mode is changed, all data stored in WxBeacon2 will be lost.
 */
bool changeBeaconMode()
{
    if(!wb2Client) { WB2_LOGE("Client null"); return false; }
    if((uint64_t)wb2address == 0) { WB2_LOGE("Address not yet obtained"); return false; }

    ScopedWB2Client wb2(*wb2Client);

    WB2_LOGI("connect to : %s", wb2address.toString().c_str());
    if(!wb2.client().isConnected() && !wb2.client().connect(wb2address))
    {
        WB2_LOGE("Failed to connect");
        return false;
    }

    WxBeacon2::ADVSetting setting;
    if(!wb2.client().getADVSetting(setting))
    {
        WB2_LOGE("Failed ti get setting");
        return false;
    }

    setting._beaconMode = (uint8_t)WxBeacon2::BeaconMode::GeneralBroadcaster2;

    if(wb2.client().setADVSetting(setting))
    {
        return wb2.client().setMeasurementInterval(MEASUREMENT_INTERVAL_BROADCAST_SEC);
    }
    return false;
}

/*
  Change default mode (for WNI Application)
  @warning If the beacon mode is changed, all data stored in WxBeacon2 will be lost.
 */
bool changeDefaultSetting()
{
    if(!wb2Client) { WB2_LOGE("Client null"); return false; }
    if((uint64_t)wb2address == 0) { WB2_LOGE("Address not yet obtained"); return false; }

    ScopedWB2Client wb2(*wb2Client);

    WB2_LOGI("connect to : %s", wb2address.toString().c_str());
    if(!wb2.client().isConnected() && !wb2.client().connect(wb2address))
    {
        WB2_LOGE("Failed to connect");
        return false;
    }

    // Need set interval and time inforamtion,
    if(wb2.client().setADVSetting(WxBeacon2::ADVSetting::DEFAULT_SETTING))
    {
        delay(500);
        std::tm t;
        getLocalTime(&t);
        if(wb2.client().setTimeInformation(t))
        {
            delay(500);
            return wb2.client().setMeasurementInterval(WxBeacon2::MeasurementInterval::DEFAULT_VALUE);
        }
    }
    return false;
}

/* turnon LED 1 second */
bool turnOnLED()
{
    if(!wb2Client) { WB2_LOGE("Client null"); return false; }
    if((uint64_t)wb2address == 0) { WB2_LOGE("Address not yet obtained"); return false; }

    ScopedWB2Client wb2(*wb2Client);

    WB2_LOGI("connect to : %s", wb2address.toString().c_str());
    if(!wb2.client().isConnected() && !wb2.client().connect(wb2address))
    {
        WB2_LOGE("Failed to connect");
        return false;
    }
    return wb2.client().setLED(1);
}

//
TaskHandle_t advertiseTask;
WxBeacon2::AdvertiseData advertiseData;
volatile bool updatedAdvertise = false;
volatile bool advertiseBusy = false;
time_t lastUpdate = -1;

// Task of get advertise data from WxBeacon2.
void wb2_advertise_task(void*)
{
    NimBLEScan* scan = NimBLEDevice::getScan();
    WxBeacon2AdvertiseCallbacks cb;
    scan->setAdvertisedDeviceCallbacks(&cb);
    scan->setInterval(1000);
    scan->setWindow(900);
    scan->setActiveScan(true);

    for(;;)
    {
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

        cb.clear();

        WB2_LOGI("Start scanning");
        scan->start(60, false);
        while(scan->isScanning()) { delay(100); }
        WB2_LOGI("End of scanning. detected :%d", cb.detected());

        if(cb.detected())
        {
            wb2address = cb.address();
            advertiseData = cb.data();
            lastUpdate = time(nullptr);
            if(TURNON_LED) { turnOnLED(); }
        }
        updatedAdvertise = cb.detected();
        advertiseBusy = false;
        scan->clearResults();
    }
}

// Kick adverstise_task
void getAdvertiseAsync()
{
    if(!advertiseBusy)
    {
        advertiseBusy = true;
        updatedAdvertise = false;
        xTaskNotifyGive(advertiseTask);
    }
}

// Getting comms?
bool busyAdvertise() { return advertiseBusy; }

//
Avatar* avatar;
Ticker* ticker;
bool forceFace = true; // Force rendering of the face.

// play voice
void playAdvertiseData(const WxBeacon2::AdvertiseData& data)
{
    if(data.format() != WxBeacon2::ADVFormat::E)
    {
        WB2_LOGE("Illegal format");
        return;
    }
    auto de = data.getE();
    if(!de)
    {
        WB2_LOGE("Failed to get E");
        return;
    }

    auto s = formatString("kion <NUMK VAL=%3.1f>do."
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

    WB2_LOGV("PLAY:[%s]", s.c_str());
    aq_talk::playAquesTalk(s.c_str(), 110);

    auto ts = formatString("TEMP %3.1fdegC HUM %3.1f%% AL %dlx UV %3.1f PRESURE %4.1fhPa N %3.1fdB DCMFT %3.1f WBGT %3.1f   ",
                           (float)de->temperature(),
                           (float)de->relativeHumidity(),
                           (int)de->ambientLight(),
                           (float)de->uvIndex(),
                           (float)de->presure(),
                           (float)de->soundNoise(),
                           (float)de->discomfortIndex(),
                           (float)de->heatstroke()
                           );
    WB2_LOGV("TICKER:[%s]", s.c_str());
    ticker->setText(ts.c_str());
}

constexpr char DEFAULT_TICKER_TEXT[] = "WEATHEROID Type A Airi";
constexpr char NOTICE_TICKER_TEXT[] = "Press and hold C to put the WxBeacon2 into the broadcast mode.  ";

#if 0
bool test_connect()
{
    if(!wb2Client) { WB2_LOGE("Client null"); return false; }
    if((uint64_t)wb2address == 0) { WB2_LOGE("Address not yet obtained"); return false; }

    ScopedWB2Client wb2(*wb2Client);

    WB2_LOGI("connect to : %s", wb2address.toString().c_str());
    if(!wb2.client().isConnected() && !wb2.client().connect(wb2address))
    {
        WB2_LOGE("Failed to connect");
        return false;
    }

    std::string str;
    auto b = wb2.client().getDeviceName(str);
    printf("%d DeviceName:%s\n", b, str.c_str());

    WxBeacon2::GenericAccesssService::Appearance app;
    b = wb2.client().getAppearance(app);
    printf("%d Appearnce:%x\n", b, app._category);

    WxBeacon2::GenericAccesssService::PeripheralPreferredConnectionParameters params;
    b = wb2.client().getPeripheralPreferredConnectionParameters(params);
    printf("%d params:%x/%x/%x/%x\n", b,
           params._intervalMin, params._intervalMax, params._slaveLatency, params._timeout);

    b = wb2.client().getModelNumber(str);
    printf("%d ModelNumber:[%s]\n", b, str.c_str());
    b = wb2.client().getSerialNumber(str);
    printf("%d SerialNumber:[%s]\n", b, str.c_str());
    b = wb2.client().getFirmwareRevision(str);
    printf("%d FirmRev:[%s]\n", b, str.c_str());
    b = wb2.client().getHardwareRevision(str);
    printf("%d HardRev:[%s]\n", b, str.c_str());
    b = wb2.client().getManufacturerName(str);
    printf("%d ManufacturerName:[%s]\n", b, str.c_str());
    
    return true;
}

bool test_getLatestPageData()
{
    if(advertiseData.format() == WxBeacon2::ADVFormat::E) { WB2_LOGE("Not recording mode"); return false; }
    
    if(!wb2Client) { WB2_LOGE("Client null"); return false; }
    if((uint64_t)wb2address == 0) { WB2_LOGE("Address not yet obtained"); return false; }

    ScopedWB2Client wb2(*wb2Client);

    WB2_LOGI("connect to : %s", wb2address.toString().c_str());
    if(!wb2.client().isConnected() && !wb2.client().connect(wb2address))
    {
        WB2_LOGE("Failed to connect");
        return false;
    }

    WxBeacon2::LatestPage lpage;
    if(!wb2.client().getLatestPage(lpage)) { WB2_LOGE("Failed to get latest page"); return false; }
    printf("Latest: %d,%d\n", lpage._page, lpage._row);

    int retryCount = 3;
    WxBeacon2::ResponseFlag flag = { 2 };
    do
    {
        if(!wb2.client().requestPage(lpage._page, lpage._row)) { continue; } // request
        for(;;) // get response flag
        {
            wb2.client().getResponseFlag(flag);
            if((int)flag._updateFlag != 0) { break; }
            delay(10);
        }
    }while((int)flag._updateFlag == 2 && retryCount--);
    if((int)flag._updateFlag != 1) { WB2_LOGE("Failed to request or get response"); return false; }

    int cnt = lpage._row + 1;
    while(cnt--)
    {
        WxBeacon2::ResponseData data;
        if(!wb2.client().getResponseData(data)) { WB2_LOGE("Failed to get response data"); return false; }
        time_t t = flag._time32 + lpage._interval * data._data._row;
        auto lt = std::localtime(&t);
        printf("DATA: row:%02d time:%4d/%02d/%02d %02d:%02d:%02d\n",
               data._data._row,
               lt->tm_year + 1900,
               lt->tm_mon + 1,
               lt->tm_mday,
               lt->tm_hour,
               lt->tm_min,
               lt->tm_sec);
    }
    return true;
}
#endif
//
}

void setup()
{
    // log level
    esp_log_level_set("wifi", (esp_log_level_t)0);
    esp_log_level_set("NimBLE", (esp_log_level_t)0);
    esp_log_level_set("NimBLEScan", (esp_log_level_t)0);
    esp_log_level_set("NimBLEDevice", (esp_log_level_t)0);
    esp_log_level_set("NIMBLE_NVS", (esp_log_level_t)0);

    //
    M5.begin();
    auto bd = M5.getBoard();
    WB2_LOGV("board : %x", bd); // 2:M5Stack Basic/Gray 3:Core2

    auto scfg = M5.Speaker.config();
    scfg.task_priority = 2;
    scfg.task_pinned_core = 0;
    M5.Speaker.config(scfg);
    M5.Speaker.begin();
    M5.Speaker.setVolume(bd == m5::board_t::board_M5Stack ? 128 : 64);
    delay(500);

    if (M5.Display.width() < M5.Display.height())
    {
        M5.Display.setRotation(M5.Display.getRotation() ^ 1);
    }
    M5.Display.setBrightness(30);
    M5.Display.clear();

    M5.Display.fillScreen(TFT_DARKGREY);
    M5.Display.setCursor(0,0);
    M5.Display.printf("Wifi Connect");
    if(WIFI_HOST_NAME && WIFI_HOST_NAME[0])
    {
        WiFi.setHostname(WIFI_HOST_NAME);
    }

    // WiFi connect
    WiFi.begin(); // Connect to credential in Hardware. (ESP32 saves the last Wifi connection)
    int tcount = 10;
    while(tcount-- > 0 && WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      M5.Display.printf(".");
    }
    if(WiFi.status() != WL_CONNECTED)
    {
        WB2_LOGE("Failed to connect WiFi");
        abort();
    }

    // Configurate local time.
    M5.Display.setCursor(0,0);
    M5.Display.printf("Configurate time");
    
    configTime(gmtOffsetSec, daylightOffsetSec, ntpURL[0]);
    std::tm t;
    if(getLocalTime(&t))
    {
        WB2_LOGI("Configurate time : %4d/%02d/%02d (%d) %02d:%02d:%02d",
                 t.tm_year + 1900,
                 t.tm_mon + 1,
                 t.tm_mday,
                 t.tm_wday,
                 t.tm_hour,
                 t.tm_min,
                 t.tm_sec);
    }
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    M5.Display.fillScreen(TFT_DARKGREEN);

    // BLE
    NimBLEDevice::init("");
    xTaskCreateUniversal(wb2_advertise_task, "advertise_task", 8192, nullptr, 1, &advertiseTask, 1);
    assert(advertiseTask);

    // get advertise from WxBeacon2
    auto ret = aq_talk::setup();
    WB2_LOGD("aq_talk::setup : %d", ret);
    avatar = new Avatar();
    assert(avatar);

    ticker = new Ticker();
    assert(ticker);
    ticker->setText(DEFAULT_TICKER_TEXT);

    wb2Client = new WxBeacon2Client();
    assert(wb2Client);
    
    getAdvertiseAsync();
    while(busyAdvertise()) { delay(100); }
    if(!updatedAdvertise)
    {
        WB2_LOGE("Not detected WxBeacon2");
        abort();
    }
    if(updatedAdvertise && advertiseData.format() != WxBeacon2::ADVFormat::E)
    {
        WB2_LOGE("WxBeacon2 beacon mode is not broadcast2");
        ticker->setLevel(Ticker::Level::Purple);
        ticker->setText(NOTICE_TICKER_TEXT);
            
    }
    
    auto mem = esp_get_free_heap_size();
    WB2_LOGV("end of setup %u", mem);
}

namespace
{
    bool longPressA, longPressB,longPressC;
}

void loop()
{
    M5.update();

    // WARNING : If the beacon mode is changed, all data stored in WxBeacon2 will be lost.
    /*
      C button
      long press : Set beacon mode to GeneralBroadcaster2 (for me)
      press : show/hide ticker
    */
    if(!longPressC && M5.BtnC.pressedFor(1000))
    {
        longPressC = true;
        WB2_LOGI("Set beacon mode to broadcast mode");
        auto b = changeBeaconMode();
        if(b)
        {
            ticker->setLevel(Ticker::Level::Blue);
            ticker->setText(DEFAULT_TICKER_TEXT);
            ticker->show(true);
            getAdvertiseAsync();
        }
        WB2_LOGI("result : %d", b);
    }
    if(M5.BtnC.wasReleased())
    {
        if(!longPressC)
        {
            ticker->show(!ticker->isShow());
            forceFace = true;
        }
        longPressC = false;
    }

    /*
      B button
      long press : Set default setting for WNI aplication.
      press : (none)
    */
    if(!longPressB && M5.BtnB.pressedFor(1000))
    {
        longPressB = true;
        WB2_LOGI("Set default settins");
        aq_talk::stopAquesTalk();
        auto b = changeDefaultSetting();
        if(b)
        {
            ticker->setLevel(Ticker::Level::Purple);
            ticker->setText(NOTICE_TICKER_TEXT);
            ticker->show(true);
        }
        WB2_LOGI("result : %d", b);
    }
    if(M5.BtnB.wasReleased())
    {
        if(!longPressB)
        {
            // test_connect();
            // test_getLatestPageData();
            #if 0
            // TEST
            auto lv = ticker->level() + 1;
            if(lv > 3) { lv = 0; }
            ticker->setLevel((Ticker::Level)lv);
            #endif
        }
        longPressB = false;
    }

    /*
      A button
      long press : (none)
      press : Obtain data force.
    */
    if(!longPressA && M5.BtnA.pressedFor(1000))
    {
        longPressA = true;
    }
    if(M5.BtnA.wasReleased())
    {
        if(!longPressA)
        {
            if(!busyAdvertise())
            {
                aq_talk::stopAquesTalk();
                getAdvertiseAsync();
            }
        }
        longPressA = false;
    }

    // Obtain data at intervals.
    time_t t = time(nullptr);
    if(!busyAdvertise() && lastUpdate > 0 && std::difftime(t, lastUpdate) >= TALKING_INTERVAL_SEC)
    {
        getAdvertiseAsync();
    }

    // Play latest advertise data if exists.
    if(updatedAdvertise)
    {
        updatedAdvertise = false;
        playAdvertiseData(advertiseData);
        auto de = advertiseData.getE();
        if(de) {  ticker->setLevelWBGT(de->heatstroke()); }
    }

    //
    avatar->pump();
    ticker->pump();

    M5.Display.startWrite(); // Occupy DMA Bus
    avatar->render(&M5.Display, forceFace); forceFace = false;
    ticker->render(&M5.Display);
    M5.Display.endWrite(); // Do render to display

    delay(1000/30); // Keep about 30 FPS
}

