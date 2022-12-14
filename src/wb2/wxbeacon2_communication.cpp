
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "wxbeacon2_communication.hpp"
#include "wxbeacon2_ble.hpp"
#include "wxbeacon2_log.hpp"

namespace
{
constexpr uint16_t MEASUREMENT_INTERVAL_BROADCAST_SEC = 60 * 3; // Interval default.

TaskHandle_t advertiseTask;
volatile bool exists = false; // Exists WxBeacon2?
volatile bool progressAdvertise = false;
advertise_callback callback_on_advertise;

NimBLEAddress wb2address; // Detected wxbeacon2 address
WxBeacon2Client* wb2Client = nullptr;

//  WxBeacon2Client Scoped init,connect/disconnect,deinit.
struct ScopedWB2Client
{
    ScopedWB2Client(WxBeacon2Client& client, const NimBLEAddress& addr) : _client(client)
    {
        NimBLEDevice::init("");
        WB2_LOGI("connect to : %s", addr.toString().c_str());
        if(!_client.isConnected() && !_client.connect(addr))
        {
            WB2_LOGE("Failed to connect");
        }
    }
    ~ScopedWB2Client()
    {
        _client.disconnect();
        NimBLEDevice::deinit(false);
    }
    WxBeacon2Client& client() { return _client; }
  private:    
    WxBeacon2Client& _client;
};

// Task of get advertise data from WxBeacon2.
void wb2_advertise_task(void*)
{
    for(;;)
    {
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

        WB2_LOGI("Start scanning");
        NimBLEDevice::init(""); 
        NimBLEScan* scan = NimBLEDevice::getScan();
        WxBeacon2AdvertiseCallbacks cb;
        scan->setAdvertisedDeviceCallbacks(&cb);
        scan->setInterval(1000);
        scan->setWindow(900);
        scan->setActiveScan(true);
        cb.clear();

        scan->start(60, false);
        while(scan->isScanning()) { delay(100); }

        wb2address = cb.address();
        if(callback_on_advertise) { callback_on_advertise(cb.detected(), cb.data()); }

        scan->stop();
        scan->clearResults();
        NimBLEDevice::deinit(false);

        WB2_LOGI("End of scanning. detected :%d", cb.detected());
        progressAdvertise = false;
    }
}
//
}

bool initilizeAdvertise(const UBaseType_t priority, const BaseType_t core, advertise_callback f)
{
    if(!advertiseTask)
    {
        xTaskCreateUniversal(wb2_advertise_task, "advertise_task", 4 * 1024, nullptr, priority, &advertiseTask, core);
        if(!wb2Client) {   wb2Client = new WxBeacon2Client(); }
        callback_on_advertise = f;
    }
    return (wb2Client != nullptr) && (advertiseTask != nullptr);
}

bool requestAdvertise()
{
    if(busyAdvertise()) { return false; }

    progressAdvertise = true;
    xTaskNotifyGive(advertiseTask);
    return true;
}

bool busyAdvertise() { return progressAdvertise; }

/* 
  Change broadcast mode
  @warning  Changing to broadcast mode will not work with the WNI application.
  @warning If you want to link with the WNI application, call the changeDefaultSetting function.
  @warning If the beacon mode is changed, all data stored in WxBeacon2 will be lost.
 */
bool changeBeaconMode()
{
    if(!wb2Client || ((uint64_t)wb2address == 0) || busyAdvertise()) { WB2_LOGD("Cannot invoke"); return false; }

    ScopedWB2Client wb2(*wb2Client, wb2address);
    if(!wb2.client().isConnected())
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
    if(!wb2Client || ((uint64_t)wb2address == 0) || busyAdvertise()) { WB2_LOGD("Cannot invoke"); return false; }

    ScopedWB2Client wb2(*wb2Client, wb2address);
    if(!wb2.client().isConnected())
    {
        WB2_LOGE("Failed to connect");
        return false;
    }

    // Need set interval and time inforamtion,
    if(wb2.client().setADVSetting(WxBeacon2::ADVSetting::DEFAULT_SETTING))
    {
        delay(500);
        std::tm t{};
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
    if(!wb2Client || ((uint64_t)wb2address == 0) || busyAdvertise()) { WB2_LOGD("Cannot invoke"); return false; }
    
    ScopedWB2Client wb2(*wb2Client, wb2address);
    if(!wb2.client().isConnected())
    {
        WB2_LOGE("Failed to connect");
        return false;
    }
    return wb2.client().setLED(1);
}

#if 0
bool test_connect()
{
    if(!wb2Client || ((uint64_t)wb2address == 0) || busyAdvertise()) { WB2_LOGD("Cannot invoke"); return false; }

    ScopedWB2Client wb2(*wb2Client, wb2address);
    if(!wb2.client().isConnected())
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
    if(!wb2Client || ((uint64_t)wb2address == 0) || busyAdvertise()) { WB2_LOGD("Cannot invoke"); return false; }
    
    if(!wb2Client) { WB2_LOGE("Client null"); return false; }
    if((uint64_t)wb2address == 0) { WB2_LOGE("Address not yet obtained"); return false; }

    ScopedWB2Client wb2(*wb2Client, wb2address);
    if(!wb2.client().isConnected())
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
