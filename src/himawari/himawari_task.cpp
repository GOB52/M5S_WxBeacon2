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
#include "wb2/wxbeacon2_log.hpp"

namespace
{
// Request himawari image
TaskHandle_t himawariTask;
volatile bool progressHimawari = false;
himawari::image_callback callbackOnImage;

//PROGMEM const char band_b13 = "b13";
//PROGMEM const char band_snd = "snd";

//PROGMEM const char HIMAWARI_URI_FORMAT[] = "https://www.data.jma.go.jp/mscweb/data/himawari/img/jpn/jpn_%s_%s.jpg";
PROGMEM const char HIMAWARI_URI_FORMAT[] = "https://www.data.jma.go.jp/mscweb/data/himawari/img/jpn/jpn_b07_1530.jpg";
// DigiCert Global Root CA of www.data.jma.go.jp.
// Expire at Mon, 10 Nov 2031 00:00:00 GMT
PROGMEM const char root_ca_ECC[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
        "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
        "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
        "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
        "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
        "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
        "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
        "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
        "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
        "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
        "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
        "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
        "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
        "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
        "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
        "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
        "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
        "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
        "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
        "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
        "-----END CERTIFICATE-----\n";

uint8_t* jpg;


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
            if(callbackOnImage) { callbackOnImage(nullptr); }
            progressHimawari = false;
            continue;
        }

        char url[128];
        snprintf(url ,sizeof(url), HIMAWARI_URI_FORMAT);
        
        HTTPClient http;
        WiFiClientSecure client;
        do
        {
            client.setCACert(root_ca_ECC);
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

            size_t sz = http.getSize();
            WB2_LOGD("jpgsz:%zu", sz);
            jpg = (uint8_t*)malloc(sz);
            if(!jpg)
            {
                WB2_LOGD("Failed to allocate");
                break;
            }
            client.readBytes(jpg, sz);
            WB2_LOGD("av:%d" ,client.available());
        }while(0);
        http.end();
        client.stop();

        WB2_LOGD("WiFi disconnect");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);

        if(callbackOnImage) { callbackOnImage(jpg); }
        
        progressHimawari = false;
    }
}
//
}




namespace himawari
{

bool initialize(const UBaseType_t priority, const BaseType_t core, image_callback f)
{
    if(!himawariTask)
    {
        xTaskCreateUniversal(himawari_task, "himawari_task", 8 * 1024, nullptr, priority, &himawariTask, core);
        callbackOnImage = f;
    }
    return himawariTask != nullptr;
}

bool busy()
{
    return progressHimawari;
}

bool request()
{
    if(busy()) { return false; }

    progressHimawari = true;
    xTaskNotifyGive(himawariTask);
    return true;
}

//
}
#endif
