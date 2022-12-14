
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <JsonStreamingParser2.h>
#include "jma_task.hpp"
#include "jma_forecast_handler.hpp"
#include "wb2/wxbeacon2_log.hpp"

namespace
{
// Request JMA forcast by WiFi
TaskHandle_t forecastTask;
volatile bool progressForecast = false;
jma::forecast_callback callback_on_forecast;
PROGMEM const char JMA_FORECAST_URI_FORMAT[] = "https://www.jma.go.jp/bosai/forecast/data/forecast/%06d.json";
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
            continue;
        }

        // Get each JSON.
        for(uint8_t index = 0; index < sizeof(requestTable)/sizeof(requestTable[0]); ++index)
        {
            char url[128];
            snprintf(url, sizeof(url), JMA_FORECAST_URI_FORMAT, requestTable[index]);
            url[sizeof(url) - 1] = '\0';

            WB2_LOGI("Connect to [%s]", url);
        
            HTTPClient http;
            WiFiClientSecure client;
            do
            {
                client.setCACert(jma::root_ca_JMA);
                int8_t gtry = 3;
                int httpcode = 0;
                // https GET
                do
                {
                    http.begin(client, url);
                    httpcode = http.GET();
                    if(httpcode == HTTP_CODE_OK) { break; }
                    WB2_LOGD("Failed to GET() %d", httpcode);
                    http.end();
                    client.stop();
                }while(gtry--);

                if(httpcode != HTTP_CODE_OK) { break; }

                // parse JSON
                JsonStreamingParser parser;
                JMAHandler handler;
                parser.setHandler(&handler);
                while(client.available()) { parser.parse(client.read()); }

                if(callback_on_forecast) { callback_on_forecast(requestTable[index], handler._forecast, handler._weeklyForecast); }
            }while(0);

            http.end();
            client.stop();
        }
        WB2_LOGD("WiFi disconnect");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);

        // terminate callback
        if(callback_on_forecast)
        {
            JMAHandler dummy;
            callback_on_forecast(0 /* End */ , dummy._forecast, dummy._weeklyForecast);
        }

        progressForecast = false;
    }
}
//
}

namespace jma
{

bool initializeForecast(const UBaseType_t priority, const BaseType_t core, forecast_callback f)
{
    if(!forecastTask)
    {
        xTaskCreateUniversal(jma_forecast_task, "forecast_task", 8 * 1024, nullptr, priority, &forecastTask, core);
        callback_on_forecast = f;
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
