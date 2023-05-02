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
MIIDdzCCAl+gAwIBAgIBADANBgkqhkiG9w0BAQsFADBdMQswCQYDVQQGEwJKUDEl
MCMGA1UEChMcU0VDT00gVHJ1c3QgU3lzdGVtcyBDTy4sTFRELjEnMCUGA1UECxMe
U2VjdXJpdHkgQ29tbXVuaWNhdGlvbiBSb290Q0EyMB4XDTA5MDUyOTA1MDAzOVoX
DTI5MDUyOTA1MDAzOVowXTELMAkGA1UEBhMCSlAxJTAjBgNVBAoTHFNFQ09NIFRy
dXN0IFN5c3RlbXMgQ08uLExURC4xJzAlBgNVBAsTHlNlY3VyaXR5IENvbW11bmlj
YXRpb24gUm9vdENBMjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANAV
OVKxUrO6xVmCxF1SrjpDZYBLx/KWvNs2l9amZIyoXvDjChz335c9S672XewhtUGr
zbl+dp+++T42NKA7wfYxEUV0kz1XgMX5iZnK5atq1LXaQZAQwdbWQonCv/Q4EpVM
VAX3NuRFg3sUZdbcDE3R3n4MqzvEFb46VqZab3ZpUql6ucjrappdUtAtCms1FgkQ
hNBqyjoGADdH5H5XTz+L62e4iKrFvlNVspHEfbmwhRkGeC7bYRr6hfVKkaHnFtWO
ojnflLhwHyg/i/xAXmODPIMqGplrz95Zajv8bxbXH/1KEOtOghY6rCcMU/Gt1SSw
awNQwS08Ft1ENCcadfsCAwEAAaNCMEAwHQYDVR0OBBYEFAqFqXdlBZh8QIH4D5cs
OPEK7DzPMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3
DQEBCwUAA4IBAQBMOqNErLlFsceTfsgLCkLfZOoc7llsCLqJX2rKSpWeeo8HxdpF
coJxDjrSzG+ntKEju/Ykn8sX/oymzsLS28yN/HH8AynBbF0zX2S2ZTuJbxh2ePXc
okgfGT+Ok+vx+hfuzU7jBBJV1uXk3fs+BXziHV7Gp7yXT2g69ekuCkO2r1dcYmh8
t/2jioSgrGK+KwmHNPBqAbubKVY8/gA3zyNs8U6qtnRGEmyR7jTV7JqR50S+kDFy
1UkC9gLl9B/rfNmWVan/7Ir5mUf/NVoCqgTLiluHcSmRvaS0eg29mvVXIwAHIRc/
SjnRBUkLp7Y3gaVdjKozXoEofKd9J+sAro03
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
        int8_t ctry = 5;
        do
        {
            WiFi.mode(WIFI_STA);
            WiFi.begin();
            int tcount = 20;
            while(tcount-- > 0 && WiFi.status() != WL_CONNECTED) { delay(500); }

            auto s = WiFi.status();
            if(s == WL_CONNECTED) { break; }

            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            WB2_LOGD("Retry begin:%d", s);
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
                WB2_LOGE("%s", http.errorToString(httpcode).c_str());
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

            http.setTimeout(30); // seconds
            gtry = 16;
            do
            {
                while(http.connected() && client.available())
                {
                    size_t read = std::min(blockSize, szTmp);
                    WB2_LOGD("Try read:%zu", read);
                    while(read)
                    {
                        auto len = client.readBytes(jpg + off, read);
                        read -= len;
                        off += len;
                        WB2_LOGD("len:%zu off:%zu ava:%zu", len, off, client.available());
                        delay(100); // Insert appropriate delay to continue reading the stream.
                    }
                    szTmp -= std::min(blockSize, szTmp);
                    if(callbackOnProgress) { callbackOnProgress(off, jpgsz); }
                    delay(100);
                }
                WB2_LOGD("http:%d ava:%zu", http.connected(), client.available());
                if(off < jpgsz) { WB2_LOGD("Not enough read"); delay(500); }
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
