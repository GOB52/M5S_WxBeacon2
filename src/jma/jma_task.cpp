
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <JsonStreamingParser2.h>
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
// Exxpire at Mon, 10 Nov 2031 00:00:00 GMT.
PROGMEM const char root_ca_JMA[] =
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

                // parse JSON
                JsonStreamingParser parser;
                JMAHandler handler;
                parser.setHandler(&handler);
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
