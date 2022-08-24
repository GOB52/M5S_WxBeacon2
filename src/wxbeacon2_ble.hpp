/*!
  @file   wxbeacon2_ble.hpp
  @brief  wxbeacon2 BLE interface
  @note depends on NimBLE-Arduino
*/
#ifndef WXBEACON2_BLE_HPP
#define WXBEACON2_BLE_HPP

#include <NimBLEAdvertisedDevice.h>
#include <NimBLEClient.h>
#include "wxbeacon2.hpp"
#include <ctime>

/*! @brief Advertise Callbacks */
class WxBeacon2AdvertiseCallbacks: public NimBLEAdvertisedDeviceCallbacks
{
  public:
    WxBeacon2AdvertiseCallbacks() : NimBLEAdvertisedDeviceCallbacks()
                                  , _data()
    {}

    void onResult(NimBLEAdvertisedDevice* device) override;

    void clear();
    bool detected() { return static_cast<uint64_t>(_address) != 0ULL; }

    const NimBLEAddress& address() const { return _address; }
    const WxBeacon2::AdvertiseData& data() const { return _data; }

    static WxBeacon2::ADVFormat format(std::string& name)
    {
        if(name == "IM") return WxBeacon2::ADVFormat::D;
        if(name == "EP") return WxBeacon2::ADVFormat::E;
        if(name == "Env") return WxBeacon2::ADVFormat::C;
        return WxBeacon2::ADVFormat::Unknown;
    }
    
  private:
    bool validName(std::string& name) const { return name == "IM" || name == "EP" || name == "Env"; }

  private:
    NimBLEAddress _address; // detected device addrress
    WxBeacon2::AdvertiseData _data;
};

/*! @brief WxBeacon2 client interface */
class WxBeacon2Client
{
  public:
    using DataVector = std::vector<WxBeacon2::Data>;

    /*! @brief get custom service UUID from 16bit GATT. */
    static NimBLEUUID customServiceUUID(uint16_t gatt);
    /*! @brief get custom characteristics UUID from 16bit GATT. */
    static NimBLEUUID customCharacteristicsUUID(uint16_t gatt);

    /*! @brief callbacks */
    class Callbacks : public NimBLEClientCallbacks
    {
      public:
        Callbacks(WxBeacon2Client* wc) : NimBLEClientCallbacks(), _wc(wc) {}
        virtual void onConnect(NimBLEClient *pClient) override;
        virtual void onDisconnect(NimBLEClient *pClient) override;

      private:
        WxBeacon2Client* _wc;
    };

    struct PageTime
    {
        uint16_t _page;
        std::time_t _time;
        PageTime(uint16_t page, std::time_t t) : _page(page), _time(t) {}
    };
    
    //
    WxBeacon2Client();
    virtual ~WxBeacon2Client();
    NimBLEClient* client() { return _client; }
    
    /// @name Wrapper
    /// @{
    bool isConnected() { return _client->isConnected(); }
    bool connect(const NimBLEAddress& address, bool deleteAttibutes = true)
    {
        return _client && _client->connect(address, deleteAttibutes);
    }
    int disconnect(uint8_t reason = BLE_ERR_REM_USER_CONN_TERM)
    {
        return _client ? _client->disconnect(reason) : BLE_HS_EUNKNOWN;
    }
    /// @}

    /// @name Get value
    /// @{
    /*! @brief get latest data */
    bool getLatestData(WxBeacon2::LatestData& ldata);
    /*! @brief get latest page */
    bool getLatestPage(WxBeacon2::LatestPage& lpage);
    /*! @brief get request page */
    bool getRequestPage(WxBeacon2::RequestPage& req);
    /*! @brief get response flag */
    bool getResponseFlag(WxBeacon2::ResponseFlag& flag);
    // get recording data.
    bool getResponseData(DataVector& v, const uint16_t pageFrom = 0, const uint8_t rowFrom = 0);
    /*! @brief get measurement interval */
    bool getMeasurementInterval(WxBeacon2::MeasurementInterval& mi);
    /*!@brief get time inforamtion */
    bool getTimeInformation(WxBeacon2::TimeInformation& info);
    /*! @brief get error status */
    bool getErrorStatus(WxBeacon2::ErrorStatus& status);
    /*! @brief  get created time of each page. */
    bool getPageCreatedTime(std::vector<PageTime>& vec, const uint16_t from, const uint16_t to);
    /*! @brief get advertise setting */
    bool getADVSetting(WxBeacon2::ADVSetting& setting);
    /// @}
    
    /// @name Set value
    /// @{
    /*!
      @brief set request page
      @param page page for get response data
      @param row row for get response data
     */
    bool requestPage(uint16_t page, uint8_t row);
    /*!
      @brief Set measurement interval Unit: 1 sec
      @warning Time Information is cleared to zero (0) when measurement interval is changed,
      @warning so Time Information must be set to start data recording again.
    */
    bool setMeasurementInterval(const uint16_t sec);
    /*! @brief Set time information */
    bool setTimeInformation(const time_t unixTime);
    /*! @brief Set time information */
    bool setTimeInformation(const std::tm& t);
    /*! @brief Set LED on duration */
    bool setLED(const uint8_t duration);

    /*!
      @brief set error status
      @note The error state can be reset by writing 0 from the Central device.
    */
    bool setErrorStatus(const WxBeacon2::ErrorStatus& status);
    /*!
      @brief set ADV setting
      @warning Time Information is cleared to zero (0) when Beacon Mode is changed,
      @warning so Time Information must be set to start data recording again.
     */
    bool setADVSetting(const WxBeacon2::ADVSetting& setting);
    /// @}
    
  protected:
    /*!
      @brief get rows from 1 page.
      @param page Read page no.
      @param firstRow First row to retrieve
      @param lastRow Last row to retrieve
     */
    size_t _getResponseData(DataVector& vec, const uint16_t page, const uint8_t firstRow, const uint8_t lastRow, const uint16_t interval, const uint32_t retry = 3);

  private:
    WxBeacon2Client(const WxBeacon2Client&) = delete;
    WxBeacon2Client& operator=(const WxBeacon2Client&) = delete;
    WxBeacon2Client(WxBeacon2Client&&) = delete;
    WxBeacon2Client& operator=(WxBeacon2Client&&) = delete;
    
  private:
    NimBLEClient* _client;

    // base uuid for custom services,characteristics.
    static const char _customBaseUUID[];

    friend class Callbacks;
};
#endif
