
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <gob_json.hpp>
#include "jma_task.hpp"
#include "jma_forecast_handler.hpp"
#include "wb2/wxbeacon2_log.hpp"
#include "utility.hpp"

namespace
{
// Request JMA forcast by WiFi
TaskHandle_t forecastTask;
volatile bool progressForecast = false;
jma::forecast_callback callbackOnForecast;
jma::progress_callback callbackOnProgress;
PROGMEM const char JMA_FORECAST_URI_FORMAT[] = "https://www.jma.go.jp/bosai/forecast/data/forecast/%06d.json";

// DigiCert Global Root CA of www.jma.go.jp.
// If you get an X509 error, get the latest root certificate and rewrite this part.
PROGMEM const char root_ca_JMA[] =
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


// Cities from which data is obtained.
PROGMEM const jma::officecode_t requestTable[] =
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

void end()
{
    JMAHandler dummy;
    callbackOnForecast(0 /* End */ , dummy._forecast, dummy._weeklyForecast);
}

void jma_forecast_task(void*)
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
            progressForecast = false;
            end();
            continue;
        }

        // Get each JSON.
        for(uint8_t index = 0; index < gob::size(requestTable); ++index)
        {
            char url[128];
            snprintf(url, sizeof(url), JMA_FORECAST_URI_FORMAT, requestTable[index]);
            url[sizeof(url) - 1] = '\0';

            WB2_LOGI("Connect to [%s]", url);
        
            HTTPClient http;
            WiFiClientSecure client;
            client.setCACert(root_ca_JMA);
            do
            {
                int8_t gtry = 3;
                int httpcode = 0;
                // https GET
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
                    WB2_LOGD("Failed to GET() %d", httpcode);
                    break;
                }
                client.setTimeout(30); // seconds
                // parse JSON
                JMAHandler handler;
                goblib::json::StreamingParser parser(&handler);
                while(client.available()) { parser.parse(client.read()); }

                if(callbackOnForecast) { callbackOnForecast(requestTable[index], handler._forecast, handler._weeklyForecast); }
            }while(0);

            client.stop();
            http.end();
            if(callbackOnProgress) { callbackOnProgress(index + 1, gob::size(requestTable)); }
            delay(10);
        }

        WB2_LOGD("WiFi disconnect");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        end();
        progressForecast = false;
    }
}
//
}

namespace jma
{

bool initializeForecast(const UBaseType_t priority, const BaseType_t core, forecast_callback fc, progress_callback pc)
{
    if(!forecastTask)
    {
        xTaskCreateUniversal(jma_forecast_task, "forecast_task", 8 * 1024, nullptr, priority, &forecastTask, core);
        callbackOnForecast = fc;
        callbackOnProgress = pc;
    }
    return forecastTask != nullptr;
}

bool busyForecast()
{
    return progressForecast;
}

// Kick jma_forecast_task
bool requestForecast()
{
    if(busyForecast()) { return false; }

    progressForecast = true;
    xTaskNotifyGive(forecastTask);
    return true;
}

//
}
