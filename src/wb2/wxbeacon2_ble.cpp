/*!
  @file   wxbeacon2_ble.cpp
  @brief  wxbeacon2 BLE interface
  @note depends on NimBLE-Arduino
*/
#include <NimBLEDevice.h>
#include "wxbeacon2_ble.hpp"
#include "wxbeacon2_log.hpp"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <esp32-hal.h> // delay

namespace
{

template<typename, typename T> struct has_from
{
    static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
};

template<typename C, typename Ret, typename... Args> struct has_from<C, Ret(Args...)>
{
  private:
    template<typename T>
    static constexpr auto check(T*) -> typename std::is_same< decltype( std::declval<T>().from( std::declval<Args>()... ) ), Ret >::type;
    template<typename> static constexpr std::false_type check(...);
    typedef decltype(check<C>(0)) type;
  public:
    static constexpr bool value = type::value;
};

/*!
  @brief get custom characteristics value
  @tparam T value of type
  @param client Client
  @param value store target
  @param gat16 16bit uuid of custom UUID
  @retval true Succeed
  @retval false Failed
 */
template<typename T> bool getCustomCharacteristicValue(NimBLEClient* client, T& value, const uint16_t gatt16)
{
    static_assert(has_from<T, bool(const std::string&)>::value, "T must be has bool from(const std::string&)");
    assert(client);

    auto sv = client->getService(WxBeacon2Client::customServiceUUID(gatt16));
    if(!sv) { WB2_LOGE("Service not exists. %x", gatt16); return false; }
    auto v = sv->getValue(WxBeacon2Client::customCharacteristicsUUID(gatt16));

    WB2_LOGI("GATT:%x len:%zu / %zu:", gatt16, v.length(), v.size());
#if defined(WB2_LOG_LEVEL) && WB2_LOG_LEVEL >= ESP_LOG_INFO
    if(v.length() > 0)
    {
        for(size_t i = 0; i < v.length(); ++i)
        {
            printf("%02x", v[i]);
        }
        printf("\n");
    }
#endif
    return value.from(v);
}

/*!
  @brief get assigned characteristics value
  @tparam T value of type
  @param client Client
  @param value store target
  @param gat16 16bit uuid of custom UUID
  @retval true Succeed
  @retval false Failed
 */
template<typename T> bool getCharacteristicValue(NimBLEClient* client, T& value, const uint16_t sUUID, const uint16_t cUUID)
{
    static_assert(has_from<T, bool(const std::string&)>::value, "T must be has bool from(const std::string&)");
    assert(client);

    auto sv = client->getService(NimBLEUUID(sUUID));
    if(!sv) { WB2_LOGE("Service not exists. %x", sUUID); return false; }
    auto v = sv->getValue(NimBLEUUID(cUUID));;

    WB2_LOGI("GATT:%x/%x len:%zu / %zu:", sUUID, cUUID, v.length(), v.size());
#if defined(WB2_LOG_LEVEL) && WB2_LOG_LEVEL >= ESP_LOG_INFO
    if(v.length() > 0)
    {
        for(size_t i = 0; i < v.length(); ++i)
        {
            printf("%02x", v[i]);
        }
        printf("\n");
    }
#endif
    return value.from(v);
}
//
}

//
void WxBeacon2AdvertiseCallbacks::clear()
{
    _address = NimBLEAddress();
    _data = WxBeacon2::AdvertiseData();
}

void WxBeacon2AdvertiseCallbacks::onResult(NimBLEAdvertisedDevice* device)
{
    auto dname = device->getName();
    auto mdata = device->getManufacturerData();
    bool detected = false;
    WB2_LOGV("devicename:%s companyID:%x", dname.c_str(), mdata.empty() ? 0 : *(const uint16_t*)mdata.data());
    if(validName(dname) && !mdata.empty() && *(const uint16_t*)mdata.data() == WxBeacon2::COMPANY_ID)
    {
        WB2_LOGI("Device <%s> ----------------", device->toString().c_str());
        WB2_LOGI("  Service UUID : %s", device->haveServiceUUID() ? device->getServiceUUID().toString().c_str() : "None");
        WB2_LOGI("  Address : %s", device->getAddress().toString().c_str());
        WB2_LOGI("  Name : %s", device->getName().c_str());

        auto fmt = format(dname, mdata.length());
        _data.from(fmt, reinterpret_cast<const uint8_t*>(mdata.data()), mdata.length());
        
        // EP
        if(fmt == WxBeacon2::ADVFormat::E)
        {
            auto e = _data.getE();
            assert(e);
            WB2_LOGI("Format E ----\n"
                     "companyID : %x\n"
                     "sequence : %u\n"
                     "temperature : %2.2f C\n"
                     "relativeHumidity : %2.2f %%\n"
                     "ambientLight : %d lx\n"
                     "uvIndex : %1.2f\n"
                     "presure : %4.2f hPa\n"
                     "sound noise : %3.2f dB\n"
                     "discomfortIndex : %2.2f\n"
                     "heatstroke : %2.2f C\n"
                     "BatteryVoltage : %u mv",
                     e->_companyID,
                     e->_sequence,
                     e->temperature(),
                     e->relativeHumidity(),
                     e->ambientLight(),
                     e->uvIndex(),
                     e->presure(),
                     e->soundNoise(),
                     e->discomfortIndex(),
                     e->heatstroke(),
                     e->batteryVoltage());
            detected = (e->_companyID == WxBeacon2::COMPANY_ID);
        }
        // IM
        else if(fmt == WxBeacon2::ADVFormat::D)
        {
            auto d = _data.getD();
            assert(d);
            WB2_LOGI("Format D ----\n"
                     "companyID : %x\n"
                     "sequence : %u\n"
                     "temperature : %2.2f C\n"
                     "relativeHumidity : %2.2f %%\n"
                     "ambientLight : %d lx\n"
                     "uvIndex : %1.2f\n"
                     "presure : %4.2f hPa\n"
                     "sound noise : %3.2f dB\n"
                     "accelerationX : %3.1f\n"
                     "accelerationY : %3.1f\n"
                     "accelerationZ : %3.1f\n" 
                     "BatteryVoltage : %u mv",
                     d->_companyID,
                     d->_sequence,
                     d->temperature(),
                     d->relativeHumidity(),
                     d->ambientLight(),
                     d->uvIndex(),
                     d->presure(),
                     d->soundNoise(),
                     d->accelerationX(),
                     d->accelerationY(),
                     d->accelerationZ(),
                     d->batteryVoltage());
            detected = (d->_companyID == WxBeacon2::COMPANY_ID);
        }
        // Env
        else if(fmt == WxBeacon2::ADVFormat::C)
        {
            auto c = _data.getC();
            assert(c);
            WB2_LOGI("Format C ----\n"
                     "companyID : %x\n"
                     "page : %u\n"
                     "row: %u\n"
                     "uid : %04x\n"
                     "tEvt : %02x\n"
                     "tEvt : %02x\n"
                     "rEvt : %02x\n"
                     "aEvt : %02x\n"
                     "pEvt : %02x\n"
                     "sEvt : %02x\n"
                     "dEvt : %02x\n"
                     "hEvt : %02x\n"
                     "mEvt : %02x" ,
                     c->_companyID,
                     c->page(),
                     c->row(),
                     c->_uniqueIdentifier,
                     c->_temperatureEvt,
                     c->_relativeHumidityEvt,
                     c->_ambientLightEvt,
                     c->_uvIndexEvt,
                     c->_presureEvt,
                     c->_soundNoiseEvt,
                     c->_discomfortIndexEvt,
                     c->_heatstrokeEvt,
                     c->_miscEvt);
            detected = (c->_companyID == WxBeacon2::COMPANY_ID);
        }

        WB2_LOGI("WxBeacon2 %s detected", detected ? "" : "NOT");

        if(detected)
        {
            _address = device->getAddress();
            NimBLEDevice::getScan()->stop();
        }
    }
}

//
void WxBeacon2Client::Callbacks::onConnect(NimBLEClient *pclient)
{
    WB2_LOGI("CONNECT");
}
void WxBeacon2Client::Callbacks::onDisconnect(NimBLEClient *pClient)
{
    WB2_LOGI("DISCONNECT");
}

//
const char WxBeacon2Client::_customBaseUUID[] = "0c4c%04x-7700-46f4-aa96-d5e974e32a54";

NimBLEUUID WxBeacon2Client::customServiceUUID(uint16_t uuid)
{
    return customCharacteristicsUUID(uuid & 0xFFF0);
}

NimBLEUUID WxBeacon2Client::customCharacteristicsUUID(uint16_t uuid)
{
    char buf[BLE_UUID_STR_LEN] = {0,};
    snprintf(buf, sizeof(buf), _customBaseUUID, uuid);
    return NimBLEUUID(std::string(buf));
}

WxBeacon2Client::WxBeacon2Client()
        : _client(nullptr)
{
    _client = BLEDevice::createClient();
    assert(_client && "client null");
    _client->setClientCallbacks(new Callbacks(this));
}

WxBeacon2Client::~WxBeacon2Client()
{
    if(_client)
    {
        BLEDevice::deleteClient(_client);
    }
}

// Sensor Service (Service UUID: 0x3000)
bool WxBeacon2Client::getLatestData(WxBeacon2::LatestData& ldata)
{
    return getCustomCharacteristicValue(_client, ldata, WxBeacon2::LatestData::UUID);
}

bool WxBeacon2Client::getLatestPage(WxBeacon2::LatestPage& lpage)
{
    return getCustomCharacteristicValue(_client, lpage, WxBeacon2::LatestPage::UUID);
}

bool WxBeacon2Client::getRequestPage(WxBeacon2::RequestPage& req)
{
    return getCustomCharacteristicValue(_client, req, WxBeacon2::RequestPage::UUID);
}

bool WxBeacon2Client::requestPage(uint16_t page, uint8_t row)
{
    assert(_client);

    WxBeacon2::RequestPage rp(page, row);
    WB2_LOGI("page:%u row:%u", rp._page, rp._row);

    auto sv = _client->getService(customServiceUUID(WxBeacon2::RequestPage::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }
    return sv->setValue(customCharacteristicsUUID(WxBeacon2::RequestPage::UUID), static_cast<std::string>(rp));
}

bool WxBeacon2Client::getResponseFlag(WxBeacon2::ResponseFlag& flag)
{
    return getCustomCharacteristicValue(_client, flag, WxBeacon2::ResponseFlag::UUID);
}

bool WxBeacon2Client::getResponseData(WxBeacon2::ResponseData& data)
{
    return getCustomCharacteristicValue(_client, data, WxBeacon2::ResponseData::UUID);
}

bool WxBeacon2Client::getResponseData(DataVector& vec, const uint16_t pageFrom, const uint8_t rowFrom)
{
    assert(_client);
    assert(pageFrom < WxBeacon2::PAGE_MAX && "Invalid pageFrom");
    assert(rowFrom < WxBeacon2::ROW_MAX && "Invalid pageRow");

    vec.clear();

    WxBeacon2::LatestPage lpage;
    if(!getLatestPage(lpage))
    {
        WB2_LOGE("Failed to get latest page");
        return false;
    }
    if(lpage._time32 == 0)
    {
        WB2_LOGE("Illegal time of latest page");
        return false;
    }
    const uint16_t pageTo = lpage._page;
    uint16_t page = pageFrom;

    const size_t cnt = WxBeacon2::calculateRows(pageFrom, rowFrom, pageTo, lpage._row);
    WB2_LOGI("from:(%u,%u) - to:(%u,%u) time: %u interval:%u", pageFrom, rowFrom, pageTo, lpage._row, lpage._time32, lpage._interval);

    for(;;)
    {
        WB2_LOGV("try get page %u", page);
        uint8_t firstRow = (page == pageFrom) ? rowFrom : 0;
        uint8_t lastRow = (page == pageTo) ? lpage._row : WxBeacon2::ROW_MAX - 1;
        auto sz = _getResponseData(vec, page, firstRow, lastRow, lpage._interval);
        if(sz != lastRow - firstRow + 1) { break; }
        if(page == pageTo) { break; }
        if(++page > WxBeacon2::PAGE_MAX) { page = 0; } // Pages are circulating
    }

    WB2_LOGI("cnt %zu size %zu", cnt, vec.size());
    return vec.size() == cnt;
}

size_t WxBeacon2Client::_getResponseData(DataVector& vec, const uint16_t page, const uint8_t firstRow, const uint8_t lastRow, const uint16_t interval, const uint32_t retry)
{
    assert(_client);
    assert(page < WxBeacon2::PAGE_MAX && "Invalid pageFrom");
    assert(firstRow < WxBeacon2::ROW_MAX && "Invalid first row");
    assert(lastRow < WxBeacon2::ROW_MAX && "Invalid last row");
    assert(lastRow >= firstRow && "lastRow must be firstRow or greater than firstRow");
    assert(interval >= WxBeacon2::INTERVAL_LOWEST && "Invalid interval");
    assert(interval <= WxBeacon2::INTERVAL_HIGHEST && "Invalid interval");

    WB2_LOGI("page:%d, first:%u last:%u", page, firstRow, lastRow);

    auto sv = _client->getService(customServiceUUID(WxBeacon2::ResponseData::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return 0; }
    
    std::vector<WxBeacon2::ResponseData> tmp;
    tmp.reserve(WxBeacon2::ROW_MAX);
    uint32_t rcnt = retry;

    while(rcnt--)
    {
        // 1) Write request page
        WB2_LOGV("Request %u,%u", page, lastRow);
        if(!requestPage(page, lastRow)) { WB2_LOGE("Failed to request"); continue; }

        // 2) Read response flag
        WxBeacon2::ResponseFlag flag = { (uint8_t)WxBeacon2::ResponseFlag::UpdateFlag::FailedToRetrieveData };
        do
        {
            delay(50); // Avoid "Controller not ready to receive packets" in ble_hci_trans_hs_acl_tx()
            if(!getResponseFlag(flag)) { WB2_LOGE("Failed to get respose flag"); break; }
        }while(flag._updateFlag == WxBeacon2::ResponseFlag::UpdateFlag::Retrieving);

        WB2_LOGD("updateFlag %x", flag._updateFlag);
        
        if(flag._updateFlag != WxBeacon2::ResponseFlag::UpdateFlag::Completed) { WB2_LOGD("flag is not completed"); continue; }

        WB2_LOGI("read data. created time: %u", flag._time32);
        
        // 3) Read response data
        /* [Blockquotes]
          Also, by reading this Characteristic, the data of the next row in the same page is automatically set to this Characteristic (descending order Row 12 to Row 0). Therefore, it is unnecessary to specify 2.1.3 Request page for each row, and all row in the same page can be read by continuous Read of this Characteristic.
         */
        uint8_t cnt = WxBeacon2::ROW_MAX;
        while(cnt--)
        {
            auto v  = sv->getValue(customCharacteristicsUUID(WxBeacon2::ResponseData::UUID));
            if(v.empty()) { WB2_LOGE("Failed to get response data"); }
            
            WxBeacon2::ResponseData rd;
            if(!rd.from(v) || rd._data._row >= WxBeacon2::ROW_MAX) { break; } // Invalid data

            if(rd._data._row >= firstRow && rd._data._row <= lastRow)
            {
                WB2_LOGV("store row:%u", rd._data._row);
                tmp.push_back(rd);
            }
            if(rd._data._row <= firstRow || rd._data._row == 0) { break; } // No more rows on the page
        }

        // Sort by row asend and push back Data.
        if(tmp.size() > 1)
        {
            std::sort(tmp.begin(), tmp.end(), [](WxBeacon2::ResponseData& a, WxBeacon2::ResponseData& b)
            {
                return a._data._row < b._data._row;
            });
        }
        for(auto& e : tmp)
        {
            vec.emplace_back(static_cast<std::time_t>(flag._time32), interval, e);
        }
        break;
    }
    if(tmp.empty()) { WB2_LOGE("Failed to get %d", rcnt); }
    WB2_LOGI("container %zu", tmp.size());
    return tmp.size();
}

bool WxBeacon2Client::getPageCreatedTime(std::vector<PageTime>& vec, const uint16_t from, const uint16_t to)
{
    assert(_client);
    assert(from < WxBeacon2::PAGE_MAX && "Invalid from");
    assert(to < WxBeacon2::PAGE_MAX && "Invalid to");
    assert(from <= to && "to must be from or greater than from");

    vec.clear();
    vec.reserve(to - from + 1);

    for(uint16_t page = from; page <= to; ++page)
    {
        int retry = 3;
        while(retry--)
        {
            if(requestPage(page, 0))
            {
                #if 0
                WxBeacon2::RequestPage req;
                WB2_LOGD("getReq");
                getRequestPage(req);
                WB2_LOGD("req : %u, %u", req._page, req._row);
                #endif
                delay(50); // Avoid "Controller not ready to receive packets" in ble_hci_trans_hs_acl_tx()

                WxBeacon2::ResponseFlag flag = { (uint8_t)WxBeacon2::ResponseFlag::UpdateFlag::FailedToRetrieveData };
                do
                {
                    if(!getResponseFlag(flag)) { WB2_LOGD("Failed to getResponseFlag"); break; }
                    delay(10);
                }while(flag._updateFlag == WxBeacon2::ResponseFlag::UpdateFlag::Retrieving);

                WB2_LOGD("%02x:%08x", flag._updateFlag, flag._time32);

                if(flag._updateFlag != WxBeacon2::ResponseFlag::UpdateFlag::Completed) { continue; }

                WB2_LOGI("emplace_back %u", page);
                vec.emplace_back(page, static_cast<std::time_t>(flag._time32));
                break;
            }
        }
    }
    WB2_LOGI("cnt %zu size %zu", to - from + 1, vec.size());
    return vec.size() == to - from + 1;
}

// Setting Service (Service UUID: 0x3010)
bool WxBeacon2Client::getMeasurementInterval(WxBeacon2::MeasurementInterval& mi)
{
    return getCustomCharacteristicValue(_client, mi, WxBeacon2::MeasurementInterval::UUID);
}

bool WxBeacon2Client::setMeasurementInterval(const uint16_t sec)
{
    assert(_client);
    assert(sec >= WxBeacon2::INTERVAL_LOWEST && "Invalid sec(min)");
    assert(sec <= WxBeacon2::INTERVAL_HIGHEST && "Invalid sec(max)");

    auto sv = _client->getService(customServiceUUID(WxBeacon2::MeasurementInterval::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }    

    WxBeacon2::MeasurementInterval mi(sec);
    return sv->setValue(customCharacteristicsUUID(WxBeacon2::MeasurementInterval::UUID), static_cast<std::string>(mi));
}


// Control Service (Service UUID: 0x3030)
bool WxBeacon2Client::getTimeInformation(WxBeacon2::TimeInformation& info)
{
    return getCustomCharacteristicValue(_client, info, WxBeacon2::TimeInformation::UUID);
}

bool WxBeacon2Client::setTimeInformation(const time_t unixTime)
{
    assert(_client);

    // If mktime was failed, return -1 of time_t.
    if(unixTime == (time_t)-1) { WB2_LOGE("Illegal time"); return false; }

    WB2_LOGI("%s : %lu", __func__, unixTime);
    
    auto sv = _client->getService(customServiceUUID(WxBeacon2::TimeInformation::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }

    WxBeacon2::TimeInformation ti(unixTime);
    return sv->setValue(customCharacteristicsUUID(WxBeacon2::TimeInformation::UUID), static_cast<std::string>(ti));
}

bool WxBeacon2Client::setTimeInformation(const std::tm& t)
{
    std::tm tmp = t;
    return setTimeInformation(std::mktime(&tmp));
}

bool WxBeacon2Client::setLED(const uint8_t duration)
{
    assert(_client);

    auto sv = _client->getService(customServiceUUID(WxBeacon2::LED::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }

    uint8_t d = duration;
    if(d < WxBeacon2::LED::DURATION_MIN) { d = WxBeacon2::LED::DURATION_MIN; }
    if(d > WxBeacon2::LED::DURATION_MAX) { d = WxBeacon2::LED::DURATION_MAX; }

    WxBeacon2::LED led(d);
    return sv->setValue(customCharacteristicsUUID(WxBeacon2::LED::UUID), static_cast<std::string>(led));
}

bool WxBeacon2Client::getErrorStatus(WxBeacon2::ErrorStatus& status)
{
    return getCustomCharacteristicValue(_client, status, WxBeacon2::ErrorStatus::UUID);
}

bool WxBeacon2Client::setErrorStatus(const WxBeacon2::ErrorStatus& status)
{
    assert(_client);

    auto sv = _client->getService(customServiceUUID(WxBeacon2::ErrorStatus::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }
    return sv->setValue(customCharacteristicsUUID(WxBeacon2::ErrorStatus::UUID), static_cast<std::string>(status));
}

// Parameter Service (Service UUID: 0x3040)
bool WxBeacon2Client::getADVSetting(WxBeacon2::ADVSetting& setting)
{
    return getCustomCharacteristicValue(_client, setting, WxBeacon2::ADVSetting::UUID);
}

bool WxBeacon2Client::setADVSetting(const WxBeacon2::ADVSetting& setting)
{
    assert(_client);
    
    auto sv = _client->getService(customServiceUUID(WxBeacon2::ADVSetting::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }
    return sv->setValue(customCharacteristicsUUID(WxBeacon2::ADVSetting::UUID), static_cast<std::string>(setting));
}

// Generic Access Service
bool WxBeacon2Client::getDeviceName(std::string& dname)
{
    assert(_client);
    dname.clear();
    auto sv = _client->getService(NimBLEUUID(WxBeacon2::GenericAccesssService::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }
    dname = sv->getValue(NimBLEUUID(WxBeacon2::GenericAccesssService::DeviceName::UUID));
    return !dname.empty();
}

bool WxBeacon2Client::getAppearance(WxBeacon2::GenericAccesssService::Appearance& app)
{
    return getCharacteristicValue(_client, app, WxBeacon2::GenericAccesssService::UUID, WxBeacon2::GenericAccesssService::Appearance::UUID);
}

bool WxBeacon2Client::getPeripheralPreferredConnectionParameters(WxBeacon2::GenericAccesssService::PeripheralPreferredConnectionParameters& params)
{
    return getCharacteristicValue(_client, params, WxBeacon2::GenericAccesssService::UUID, WxBeacon2::GenericAccesssService::PeripheralPreferredConnectionParameters::UUID);
}

// Device Information Service
bool WxBeacon2Client::getModelNumber(std::string& mnum)
{
    assert(_client);
    mnum.clear();
    auto sv = _client->getService(NimBLEUUID(WxBeacon2::DeviceInformationService::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }
    mnum = sv->getValue(NimBLEUUID(WxBeacon2::DeviceInformationService::ModelNumber::UUID));
    return !mnum.empty();
}

bool WxBeacon2Client::getSerialNumber(std::string& snum)
{
    assert(_client);
    snum.clear();
    auto sv = _client->getService(NimBLEUUID(WxBeacon2::DeviceInformationService::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }
    snum = sv->getValue(NimBLEUUID(WxBeacon2::DeviceInformationService::SerialNumber::UUID));
    return !snum.empty();
}

bool WxBeacon2Client::getFirmwareRevision(std::string& frev)
{
    assert(_client);
    frev.clear();
    auto sv = _client->getService(NimBLEUUID(WxBeacon2::DeviceInformationService::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }
    frev = sv->getValue(NimBLEUUID(WxBeacon2::DeviceInformationService::FirmwareRevision::UUID));
    return !frev.empty();
}

bool WxBeacon2Client::getHardwareRevision(std::string& hrev)
{
    assert(_client);
    hrev.clear();
    auto sv = _client->getService(NimBLEUUID(WxBeacon2::DeviceInformationService::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }
    hrev = sv->getValue(NimBLEUUID(WxBeacon2::DeviceInformationService::HardwareRevision::UUID));
    return !hrev.empty();
}

bool WxBeacon2Client::getManufacturerName(std::string& mname)
{
    assert(_client);
    mname.clear();
    auto sv = _client->getService(NimBLEUUID(WxBeacon2::DeviceInformationService::UUID));
    if(!sv) { WB2_LOGE("Service not exists"); return false; }
    mname = sv->getValue(NimBLEUUID(WxBeacon2::DeviceInformationService::ManufacturerName::UUID));
    return !mname.empty();
}
