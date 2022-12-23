/*
  Request image of Weather Satellite "Himawari"
  M5Stack Core2 only
*/
#ifdef ARDUINO_M5STACK_Core2

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "himawari_task.hpp"
#include "utility.hpp"
#include "wb2/wxbeacon2_log.hpp"

using goblib::datetime::ZoneOffset;
using goblib::datetime::LocalDateTime;
using goblib::datetime::OffsetDateTime;

namespace
{
TaskHandle_t himawariTask;
volatile bool progressHimawari = false;
volatile himawari::Band requestBand;
volatile himawari::Area requestArea;
volatile int8_t requestHour;
volatile int8_t requestMinute;
OffsetDateTime requestOdt;
himawari::image_callback callbackOnImage;
himawari::progress_callback callbackOnProgress;
uint8_t* jpg;
constexpr size_t blockSize = 1024 * 4;
constexpr time_t ADJUST_GET_TIME = 30 * 60;

PROGMEM const char band_B13[] = "b13";
PROGMEM const char band_B03[] = "b03";
PROGMEM const char band_B08[] = "b08";
PROGMEM const char band_B07[] = "b07";
PROGMEM const char band_DayMicrophysicsRGB[] = "dms";
PROGMEM const char band_NightMicrophysicsRGB[] = "ngt";
PROGMEM const char band_DustRGB[] = "dst";
PROGMEM const char band_AirmassRGB[] = "arm";
PROGMEM const char band_DaySnowFogRGB[] = "dsl";
PROGMEM const char band_NaturalColorRGB[] = "dnc";
PROGMEM const char band_TrueColorRGB[] = "tre";
PROGMEM const char band_TrueColorReproductionImage[] = "trm";
PROGMEM const char band_DayConvectiveStormRGB[] = "cve";
PROGMEM const char band_Sandwich[] = "snd";
PROGMEM const char band_B03CombinedWithB13[] = "vir";
PROGMEM const char band_B03AndB13AtNight[] = "irv";
PROGMEM const char band_HeavyRainfallPotentialAreas[] = "hrp";

PROGMEM const char* bandTable[] =
{
    band_B13,
    band_B03,
    band_B08,
    band_B07,
    band_DayMicrophysicsRGB,
    band_NightMicrophysicsRGB,
    band_DustRGB,
    band_AirmassRGB,
    band_DaySnowFogRGB,
    band_NaturalColorRGB,
    band_TrueColorRGB,
    band_TrueColorReproductionImage,
    band_DayConvectiveStormRGB,
    band_Sandwich,
    band_B03CombinedWithB13,
    band_B03AndB13AtNight,
    band_HeavyRainfallPotentialAreas,
};

PROGMEM const char area_FullDisk[] = "fd_";
PROGMEM const char area_Japan[] = "jpn";
PROGMEM const char area_Australia[] = "aus";
PROGMEM const char area_NewZealand[] = "nzl";

PROGMEM const char* areaTable[] =
{
    area_FullDisk,
    area_Japan,
    area_Australia,
    area_NewZealand
};

PROGMEM const char HIMAWARI_URI_FORMAT[] = "https://www.data.jma.go.jp/mscweb/data/himawari/img/%s/%s_%s_%02d%02d.jpg";
// DigiCert Global Root CA of www.data.jma.go.jp.
// Expire at Mon, 10 Nov 2031 00:00:00 GMT
PROGMEM const char root_ca_ECC[] =
        R"***(-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)***";

void failed()
{
    if(callbackOnImage) { callbackOnImage(nullptr, requestBand, requestOdt); }
    progressHimawari = false;
}

void himawari_task(void*)
{
    for(;;)
    {
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

        // Connect WiFi
        int8_t ctry = 3;
        do
        {
            WiFi.mode(WIFI_STA);
            WiFi.begin();
            int tcount = 20;
            while(tcount-- > 0 && WiFi.status() != WL_CONNECTED)
            {
                delay(500);
            }
            if(WiFi.status() == WL_CONNECTED) { break; }
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            WB2_LOGD("Retry begin");
            delay(10);
        }while(ctry--);

        if(WiFi.status() != WL_CONNECTED)
        {
            WB2_LOGE("Failed to connect WiFi");
            failed();
            continue;
        }

        // Get image
        size_t jpgsz{}, off{};
        HTTPClient http;
        WiFiClientSecure client;
        client.setCACert(root_ca_ECC);
        do
        {
            char url[128];
            snprintf(url ,sizeof(url), HIMAWARI_URI_FORMAT,
                     areaTable[gob::to_underlying(requestArea)],
                     areaTable[gob::to_underlying(requestArea)],
                     bandTable[gob::to_underlying(requestBand)],
                     requestHour, requestMinute);
            url[sizeof(url)-1] = '\0';
            WB2_LOGI("Connect to [%s]", url);

            int8_t gtry = 3;
            int httpcode = 0;
            do
            {
                if(!http.begin(client, url))
                {
                    WB2_LOGE("Failed to begin");
                    continue;
                }
                httpcode = http.GET();
                if(httpcode >= 0) { break; }
                // HTTPClient internal error, to retry.
                client.stop();
                http.end();
                delay(100);
            }while(gtry--);

            if(httpcode != HTTP_CODE_OK)
            {
                WB2_LOGE("Failed to GET %d", httpcode);
                failed();
                break;
            }

            size_t szTmp = jpgsz = http.getSize();
            WB2_LOGD("jpgsz:%zu ava:%zu", jpgsz, client.available());

            jpg = static_cast<uint8_t*>(malloc(jpgsz));
            if(!jpg)
            {
                WB2_LOGE("Failed to allocate");
                failed();
                break;
            }

            gtry = 8;
            do
            {
                while(http.connected() && client.available())
                {
                    size_t read = std::min(blockSize, szTmp);
                    while(read)
                    {
                        auto len = client.readBytes(jpg + off, read);
                        read -= len;
                        off += len;
                        WB2_LOGD("len:%zu off:%zu ava:%zu", len, off, client.available());
                        delay(50); // Insert appropriate delay to continue reading the stream.
                    }
                    szTmp -= std::min(blockSize, szTmp);
                    if(callbackOnProgress) { callbackOnProgress(off, jpgsz); }
                    delay(50);
                }
                WB2_LOGD("http:%d", http.connected());
                if(off < jpgsz) { WB2_LOGD("Not enough read"); delay(200); }
            }while(off < jpgsz && gtry--);

            WB2_LOGD("off:%zu/%zu", off, jpgsz);
        }while(0);

        client.stop();
        http.end();
        
        WB2_LOGD("WiFi disconnect");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);

        if(callbackOnImage) { callbackOnImage((off == jpgsz) ? jpg : nullptr, requestBand, requestOdt); }
        
        progressHimawari = false;
    }
}

bool request(const LocalDateTime& ldt, const himawari::Area area, const himawari::Band band)
{
    if(himawari::busy()) { return false; }

    if(jpg) {free(jpg); jpg = nullptr; }

    // Himawari images are generated every 10 minutes.
    // Latest is more than 20~30 minutes ago.
    auto ldt2 = LocalDateTime::ofEpochSecond((ldt.toEpochSecond(ZoneOffset::UTC) - ADJUST_GET_TIME) / 600 * 600, ZoneOffset::UTC);
    WB2_LOGI("%s", ldt2.toString().c_str());
    
    requestBand = band;
    requestArea = area;
    requestHour = ldt2.hour();
    requestMinute = ldt2.minute();
    progressHimawari = true;
    xTaskNotifyGive(himawariTask);
    return true;
}
//
}

namespace himawari
{

bool initialize(const UBaseType_t priority, const BaseType_t core, image_callback ic, progress_callback pc)
{
    if(!himawariTask)
    {
        xTaskCreateUniversal(himawari_task, "himawari_task", 8 * 1024, nullptr, priority, &himawariTask, core);
        callbackOnImage = ic;
        callbackOnProgress = pc;
    }
    return himawariTask != nullptr;
}

bool busy()
{
    return progressHimawari;
}


bool request(const goblib::datetime::OffsetDateTime& odt, const Area area, const Band band)
{
    // Convert to local date-time in UTC.
    auto ldt = odt.withOffsetSameEpoch(ZoneOffset::UTC).toLocalDateTime();
    auto ldt2 = LocalDateTime::ofEpochSecond((ldt.toEpochSecond(ZoneOffset::UTC) - ADJUST_GET_TIME) / 600 * 600, ZoneOffset::UTC);

    requestOdt = OffsetDateTime::of(ldt2, ZoneOffset::UTC).withOffsetSameEpoch(odt.offset());
    
    WB2_LOGI("%s => %s", odt.toString().c_str(), ldt.toString().c_str());
    WB2_LOGI("odt:%s", requestOdt.toString().c_str());
    return ::request(ldt, area, band);
}
//
}
#endif
