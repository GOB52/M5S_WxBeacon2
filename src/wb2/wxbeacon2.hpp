/*!
  @file   wxbeacon2.hpp
  @brief  WxBeacon2 definitions [OMRON 2JCIE-BL01]
*/
#ifndef WXBEACON2_HPP
#define WXBEACON2_HPP

#include <cstdint>
#include <cstddef>
#include <string>
#include <array>
#include <ctime>

/*! @brief Definitions for WxBeacon2 */
class WxBeacon2
{
  public:
    /// @name Constants
    /// @{
    constexpr static uint16_t COMPANY_ID = 0x02d5; // Company ID of OMRON Corporation
    constexpr static uint16_t IBEACON_COMPANY_ID = 0x004c; // Company ID of Apple, Inc.
    constexpr static uint16_t PAGE_MAX = 2048; // [0-2047]
    constexpr static uint8_t ROW_MAX = 13; // [0-12]
    constexpr static uint16_t SEQUENCE_MAX = 255; // [0-255]
    constexpr static uint16_t INTERVAL_LOWEST = 1;
    constexpr static uint16_t INTERVAL_HIGHEST = 3600;
    /// @}

    /*! @brief BeaconMode */
    enum class BeaconMode : uint8_t
    {// BeaconMode,             Record?,"Shortened",    "Device Name",          "Advertise format | event detected"
        EventBeaconScanRSP,     // Y,   "Env",          "EnvSensor-BL01",       (B) | (A)/(B) Alternate
        StandardBeacon,         // Y,   "Env",          "EnvSensor-BL01",       (B)
        GeneralBroadcaster1,    // N,   "IM",           "IM-BL01",              (D)
        LimitedBroadcaster1,    // N,   "IM",           "IM-BL01",              (D)
        GeneralBroadcaster2,    // N,   "EP",           "EP-BL01",              (E)
        LimitedBroadcaster2,    // N,   "EP",           "EP-BL01",              (E)
        Unused0,
        AlternateBeacon,        // Y,   "Env",          "EnvSensor-BL01"        (A)/(B) Alternate
        EventBeaconADV,         // Y,   "Env",          "EnvSensor-BL01"        (C) | (A)/(C) Alternate
        Max,
        Unknown = 0xFF
    };
    constexpr static bool validBeaconMode(const BeaconMode bm)
    {
        return (uint8_t)bm <= (uint8_t)BeaconMode::EventBeaconADV && bm != BeaconMode::Unused0;
    }

    /*!
      @brief Advertise format
      @note depends on BeaconMode
     */
    enum class ADVFormat : uint8_t
    {
        A, // (A) Beacon
        B, // (B) Connection Advertise 1
        C, // (C) Connection Advertise 2 (ADV_IND)
        D, // (D) Sensor ADV 1 (ADV_IND)
        E, // (E) Sensor ADV 2 (ADV_IND)
        Unknown = 0xFF
    };

    /*! @brief table of ADVFormat from beacon mode. */
    constexpr static ADVFormat formatTable[(size_t)BeaconMode::Max] =
    {
        ADVFormat::B,
        ADVFormat::B,
        ADVFormat::D,
        ADVFormat::D,
        ADVFormat::E,
        ADVFormat::E,
        ADVFormat::Unknown,
        ADVFormat::B,
        ADVFormat::C,
    };

    /*! @brief recording mode by beacon mode. true: withData, false: withoutData */
    static const bool recordingMode[(size_t)BeaconMode::Max];

    constexpr static bool withDataRecordingMode(const BeaconMode m) { return (size_t)m < sizeof(recordingMode) ? recordingMode[(uint8_t)m] :  false; }
    constexpr static bool withoutDataRecordingMode(const BeaconMode m) { return !withDataRecordingMode(m); }

    /*! @brief calculate rows between spage,srow and epage,erow. */
    constexpr static size_t calculateRows(const uint16_t spage, const uint8_t srow, const uint16_t epage, uint8_t erow)
    {
        return (spage != epage) ?
                (ROW_MAX - srow) + (erow + 1)  + (ROW_MAX * ((epage + PAGE_MAX  - spage - 1) % PAGE_MAX) ) :
                (erow >= srow) ? erow - srow + 1 : 0 /* Invalid srow,erow*/;
    }

    /*!
      @brief Temperature
      @note Unit: 0.01 degC
     */
    class Temperature
    {
      public:
        constexpr Temperature(int16_t v = 0) : _v(v) {}
        explicit operator float() const{ return (float)_v * 0.01f; }
        constexpr int16_t raw() const { return _v; }
      private:
        int16_t _v;
    } __attribute__((__packed__));
    /*!
      @brief Relative Humidity
      @note Unit: 0.01 %RH
     */
    class RelativeHumidity
    {
      public:
        constexpr RelativeHumidity(int16_t v = 0) : _v(v) {}
        explicit operator float() const{ return (float)_v * 0.01f; }
        constexpr int16_t raw() const { return _v; }
      private:
        int16_t _v;
    } __attribute__((__packed__));

    /*!
      @brief AmbientLight
      @note Unit: 1 lx
     */
    class AmbientLight
    {
      public:
        constexpr AmbientLight(int16_t v = 0) : _v(v) {}
        explicit operator int16_t() const{ return _v; }
        constexpr int16_t raw() const { return _v; }
      private:
        int16_t _v;
    } __attribute__((__packed__));
    
    /*!
      @brief UV Index
      @note Unit: 0,01
     */
    class UVIndex
    {
      public:
        constexpr UVIndex(int16_t v = 0) : _v(v) {}
        explicit constexpr operator float() const{ return (float)_v * 0.01f; }
        constexpr int16_t raw() const { return _v; }
      private:
        int16_t _v;
    } __attribute__((__packed__));

    /*!
      @brief Presure
      @note Unit: 0.1 hPa
     */
    class Presure
    {
      public:
        Presure(int16_t v = 0) : _v(v) {}
        explicit operator float() const{ return (float)_v * 0.1f; }
        int16_t raw() const { return _v; }
      private:
        int16_t _v;
    } __attribute__((__packed__));

    /*!
      @brief Sound noise
      @note Unit: 0.01 dB
     */
    class SoundNoise
    {
      public:
        SoundNoise(int16_t v = 0) : _v(v) {}
        explicit operator float() const{ return (float)_v * 0.01f; }
        int16_t raw() const { return _v; }
      private:
        int16_t _v;
    } __attribute__((__packed__));
        
    /*!
      @brief Discomfort Index
      @note Unit: 0.01
     */
    class DiscomfortIndex
    {
      public:
        DiscomfortIndex(int16_t v = 0) : _v(v) {}
        explicit operator float() const{ return (float)_v * 0.01f; }
        int16_t raw() const { return _v; }
      private:
        int16_t _v;
    } __attribute__((__packed__));

    /*!
      @brief Heatstroke risk factor
      @note Unit: 0.01 degC
     */
    class Heatstroke
    {
      public:
        Heatstroke(int16_t v = 0) : _v(v) {}
        explicit operator float() const{ return (float)_v * 0.01f; }
        int16_t raw() const { return _v; }
      private:
        int16_t _v;
    } __attribute__((__packed__));

    /*!
      @brief Acceleration
      @note Unit: 0.01
     */
    class Acceleration
    {
      public:
        Acceleration(int16_t v = 0) : _v(v) {}
        explicit operator float() const{ return (float)_v * 0.01f; }
        int16_t raw() const { return _v; }
      private:
        int16_t _v;
    } __attribute__((__packed__));
    
    /*!
      @brief Battery voltage for Advertise data
      @note Unit: 1 mv
     */
    class BatteryVoltage8
    {
      public:
        BatteryVoltage8(uint8_t v = 0) : _v(v) {}
        explicit operator uint16_t() const{ return ((uint16_t)_v + 100) * 10; }
        uint8_t raw() const { return _v; }
      private:
        uint8_t _v;
    } __attribute__((__packed__));

    /*!
      @brief Battery voltage for Response data
      @note Unit: 1 mv
    */
    class BatteryVoltage16
    {
      public:
        BatteryVoltage16(uint16_t v = 0) : _v(v) {}
        explicit operator uint16_t() const{ return _v; }
        uint16_t raw() const { return _v; }
      private:
        uint16_t _v;
    } __attribute__((__packed__));

    // Sensor Service (Service UUID: 0x3000)
    /*!
      @brief Latest data
      @note Property: READ, NOTIFY
    */
    union LatestData
    {
        constexpr static uint16_t UUID = 0x3001;

        std::array<uint8_t, 19> _array;
        struct Data
        {
            uint8_t _number; // withData: row withoutData: sequence
            Temperature _temperature;
            RelativeHumidity _relativeHumidity;
            AmbientLight _ambientLight;
            UVIndex _uvIndex;
            Presure _presure;
            SoundNoise _soundNoise;
            DiscomfortIndex _discomfortIndex;
            Heatstroke _heatstroke;
            BatteryVoltage16 _batteryVoltage;
        } __attribute__((__packed__));
        Data _data;

        LatestData() : _array{} {}

        uint8_t number() const { return _data._number; }
        float temperature() const { return static_cast<float>(_data._temperature); }
        float relativeHumidity() const { return static_cast<float>(_data._relativeHumidity); }
        int16_t ambientLight() const { return static_cast<int16_t>(_data._ambientLight); }
        float uvIndex() const { return static_cast<float>(_data._uvIndex); }
        float presure() const { return static_cast<float>(_data._presure); }
        float soundNoise() const { return static_cast<float>(_data._soundNoise); }
        float discomfortIndex() const { return static_cast<float>(_data._discomfortIndex); }
        float heatstroke() const { return static_cast<float>(_data._heatstroke); }
        uint16_t batteryVoltage() const { return static_cast<uint16_t>(_data._batteryVoltage); }

        /// @name Convert
        /// @{
        bool from(const std::string& v);
        /// @}
    };
    
    /*!
      @brief Latest page
      @note Property: READ
     */
    union LatestPage
    {
        constexpr static uint16_t UUID = 0x3002;

        std::array<uint8_t, 9> _array;
        struct
        {
            uint32_t _time32; // UNIX TIME Unit : 1 sec
            uint16_t _interval;  // Unit : 1 sec [1 - 3600]
            uint16_t _page; // [0 - 2047]
            uint8_t _row; // [0 - 12]
        } __attribute__((__packed__));

        bool localTime(std::tm& t) const;

        /// @name Convert
        /// @{
        bool from(const std::string& v);
        /// @}
    };

    /*!
      @brief Request page
      @note Property: READ, WRITE
    */
    union RequestPage
    {
        constexpr static uint16_t UUID = 0x3003;
        
        std::array<uint8_t, 3> _array;
        struct
        {
            uint16_t _page;
            uint8_t _row;
        } __attribute__((__packed__));

        RequestPage(uint16_t page = 0, uint8_t row = 0) : _page(page), _row(row) {}
        explicit RequestPage(const std::string& v) : _array{} { from(v); }
        
        /// @name Convert
        /// @{
        explicit operator std::string() const { return std::string(_array.begin(), _array.end()); }
        bool from(const std::string& v);
        /// @}
    };

    /*!
      @brief Response flag
      @note Property: READ
    */
    union ResponseFlag
    {
        constexpr static uint16_t UUID = 0x3004;

        enum class UpdateFlag : uint8_t
        {
            Retrieving,           // 0x00: Retrieving 
            Completed,            // 0x01: Completed 
            FailedToRetrieveData, // 0x02: Failed to retrieve data
        };
        
        std::array<uint8_t, 5> _array;
        struct
        {
            UpdateFlag _updateFlag;
            uint32_t _time32; // created time of page. UNIX TIME Unit: 1 sec
        } __attribute__((__packed__));

        /// @name Convert
        /// @{
        bool from(const std::string& v);
        /// @}
    };

    /*!
      @brief Response data
      @note Property: READ
    */
    union ResponseData
    {
        constexpr static uint16_t UUID = 0x3005;
        
        std::array<uint8_t, 19> _array;
        struct Data
        {
            uint8_t _row;
            Temperature _temperature;
            RelativeHumidity _relativeHumidity;
            AmbientLight _ambientLight;
            UVIndex _uvIndex;
            Presure _presure;
            SoundNoise _soundNoise;
            DiscomfortIndex _discomfortIndex;
            Heatstroke _heatstroke;
            BatteryVoltage16 _batteryVoltage;
        } __attribute__((__packed__));
        Data _data;

        ResponseData() : _array{} {}
        explicit ResponseData(const std::string& v) : _array{} { from(v); }
        
        /// @name Convert
        /// @{
        bool from(const std::string& v);
        /// @}
    };

    /*! @brief Measurement data
      made by ResponseData and UNIX TIME
     */
    struct Data
    {
        std::time_t _time; // UNIX TIME
        Temperature _temperature;
        RelativeHumidity _relativeHumidity;
        AmbientLight _ambientLight;
        UVIndex _uvIndex;
        Presure _presure;
        SoundNoise _soundNoise;
        DiscomfortIndex _discomfortIndex;
        Heatstroke _heatstroke;
        BatteryVoltage16 _batteryVoltage;

        Data(const std::time_t pageTime, const uint16_t interval, const ResponseData& rd);
        Data(const time_t t, const int16_t tmp, const int16_t hum, const int16_t light, const int16_t uv, const int16_t presure, const int16_t noise, const int16_t dis, const int16_t heat, const uint16_t battery16)
                : _time(t)
                , _temperature(tmp)
                , _relativeHumidity(hum)
                , _ambientLight(light)
                , _uvIndex(uv)
                , _presure(presure)
                , _soundNoise(noise)
                , _discomfortIndex(dis)
                , _heatstroke(heat)
                , _batteryVoltage(battery16)
        {}
        
        bool localTime(std::tm& t) const;

        float temperature() const { return static_cast<float>(_temperature); }
        float relativeHumidity() const { return static_cast<float>(_relativeHumidity); }
        int16_t ambientLight() const { return static_cast<int16_t>(_ambientLight); }
        float uvIndex() const { return static_cast<float>(_uvIndex); }
        float presure() const { return static_cast<float>(_presure); }
        float soundNoise() const { return static_cast<float>(_soundNoise); }
        float discomfortIndex() const { return static_cast<float>(_discomfortIndex); }
        float heatstroke() const { return static_cast<float>(_heatstroke); }
        uint16_t batteryVoltage() const { return static_cast<uint16_t>(_batteryVoltage); }
    };

    // Setting Service (Service UUID: 0x3010)
    /*!
      @brief Measurement interval
      @note Property: READ,WRITE
    */
    union MeasurementInterval
    {
        constexpr static uint16_t UUID = 0x3011;

        std::array<uint8_t, 2> _array;
        uint16_t _interval; // Unit: 1 sec

        explicit MeasurementInterval(uint16_t i = 0) : _interval(i) {}

        constexpr static uint16_t DEFAULT_VALUE = 300; // 300 sec => 5 min
        
        /// @name Convert
        /// @{
        explicit operator std::string() const { return std::string(_array.begin(), _array.end()); }
        bool from(const std::string& v);
        /// @}
    };

    // Control Service (Service UUID: 0x3030)
    /*!
      @brief Time information
      @note Property: READ, WRITE
    */
    union TimeInformation
    {
        constexpr static uint16_t UUID = 0x3031;

        std::array<uint8_t, 4> _array;
        uint32_t _time32; // UNIX TIME Unit : 1sec

        explicit TimeInformation(time_t t = 0) : _time32(static_cast<uint32_t>(t)) {}
        
        /// @name Convert
        /// @{
        explicit operator std::string() const { return std::string(_array.begin(), _array.end()); }
        bool from(const std::string& v);
        /// @}
    };

    /*!
      @brief LED on duration
      @note Property: WRITE
    */
    union LED
    {
        constexpr static uint16_t UUID = 0x3032;
        constexpr static uint8_t DURATION_MIN = 1;
        constexpr static uint8_t DURATION_MAX = 10;
        
        std::array<uint8_t, 1> _array;
        uint8_t _duration;

        explicit LED(uint8_t d = DURATION_MIN) : _duration(d) {}
        
        /// @name Convert
        /// @{
        explicit operator std::string() const { return std::string(_array.begin(), _array.end()); }
        /// @}
    };
    
    /*!
      @brief Error status
      @note Property: READ,WRITE
    */
    union ErrorStatus
    {
        constexpr static uint16_t UUID = 0x3033;

        // for Sensor
        constexpr static uint8_t SensorAccelerometer = 1U << 6;
        constexpr static uint8_t SensorMicrophone    = 1U << 5;
        constexpr static uint8_t SensorPresure       = 1U << 4;
        constexpr static uint8_t SensorUV            = 1U << 3;
        constexpr static uint8_t SensorLight         = 1U << 2;
        constexpr static uint8_t SensorHumidity      = 1U << 1;
        constexpr static uint8_t SensorTemperature   = 1U << 0;
        // for CPU
        constexpr static uint8_t CPUBootDefault      = 1U << 1;
        constexpr static uint8_t CPUFlashMemory      = 1U << 0;
        // for Battery
        constexpr static uint8_t BatteryRead         = 1U << 1;
        constexpr static uint8_t BatteryLow          = 1U << 0;
        
        std::array<uint8_t, 4> _array;
        struct
        {
            uint8_t _sensor;
            uint8_t _cpu;
            uint8_t _battery;
            uint8_t _rfu;
        } __attribute__((__packed__)); 
        
        ErrorStatus() : _array{} {}

        bool hasError() const { return _sensor || _cpu || _battery; }
        void clear() { _array = {}; }
        
        /// @name Convert
        /// @{
        explicit operator std::string() const { return std::string(_array.begin(), _array.end()); }
        bool from(const std::string& v);
        /// @}
    };

    // Parameter Service (Service UUID: 0x3040)
    /*!
      @brief Advertise setting
      @note Property: READ, WRITE
    */
    union ADVSetting
    {
        constexpr static uint16_t UUID = 0x3042;

        std::array<uint8_t, 10> _array;
        struct
        {
            uint16_t _adv_ind_advertiseInterval; // Unit: 0.625 ms 
            uint16_t _adv_noncon_ind_advertiseInterval; // Unit: 0.625 ms
            uint16_t _transmissionPeriodInLimitedBroadcaster; // Unit 1 sec
            uint16_t _silentPeriodInLimitedBroadCaster; // Unit 1 sec
            uint8_t _beaconMode; // WxBeacon2::BeaconMode
            int8_t _txPower; // Unit dBm
        } __attribute__((__packed__));

        float indAdvertiseInterval() const { return _adv_ind_advertiseInterval * 0.625f; }
        float nonconIndAdvertiseInterval() const { return _adv_noncon_ind_advertiseInterval * 0.625f; }
        uint16_t transmissionPeriod() const { return _transmissionPeriodInLimitedBroadcaster; }
        uint16_t silentPeriod() const { return _silentPeriodInLimitedBroadCaster; }
        WxBeacon2::BeaconMode beaconMode() const { return static_cast<WxBeacon2::BeaconMode>(_beaconMode); }
        int8_t txPower() const { return _txPower; }

        bool withDataRecordingMode() const { return WxBeacon2::withDataRecordingMode(beaconMode()); }
        bool withoutDataRecordingMode() const { return !WxBeacon2::withDataRecordingMode(beaconMode()); }

        /// @name Convert
        /// @{
        explicit operator std::string() const { return std::string(_array.begin(), _array.end()); }
        bool from(const std::string& v);
        /// @}

        const static ADVSetting DEFAULT_SETTING;
    } __attribute__((__packed__));


    //! @brief Generic Access Service (Service UUID : 0x1800)
    struct GenericAccesssService
    {
        constexpr static uint16_t UUID = 0x1800;
        //! @brief Device name
        struct DeviceName
        {
            constexpr static uint16_t UUID = 0x2A00;
        };
        //! @brief Appearance
        union Appearance
        {
            constexpr static uint16_t UUID = 0x2A01;
            std::array<uint8_t, 2> _array;
            uint16_t _category;

            bool from(const std::string& v);
        };
        //! @brief Peripheral preferred connection parameters
        union PeripheralPreferredConnectionParameters
        {
            constexpr static uint16_t UUID = 0x2A04;
            std::array<uint8_t, 8> _array;
            struct
            {
                uint16_t _intervalMin; // Minimum connection interval (Unit 1.25 ms)
                uint16_t _intervalMax; // Maximum connection interval (Unit 1.25 ms)
                uint16_t _slaveLatency; // Slave Latency
                uint16_t _timeout; // Connection supervision timeout multiplier
            } __attribute__((__packed__));

            bool from(const std::string& v);
        };
    };

    //! @brief Device Information Service (Service UUID : 0x180A)
    struct DeviceInformationService
    {
        constexpr static uint16_t UUID = 0x180A;
        //! @brief Model number
        struct ModelNumber { constexpr static uint16_t UUID = 0x2A24; };
        //! @brief Serial number
        struct SerialNumber { constexpr static uint16_t UUID = 0x2A25; };
        //! @brief Firmware revision
        struct FirmwareRevision { constexpr static uint16_t UUID = 0x2A26; };
        //! @brief Hardware revision
        struct HardwareRevision { constexpr static uint16_t UUID = 0x2A27; };
        //! @brief Manufacturer name
        struct ManufacturerName { constexpr static uint16_t UUID = 0x2A29; };
    };
    
    /*! @brief Advertise data */
    class AdvertiseData
    {
      public:
        // ADV 2 length
        constexpr static size_t LENGTH_A = 29 - 5 + 1;
        constexpr static size_t LENGTH_B = 30 - 2 + 1;
        constexpr static size_t LENGTH_C = 25 - 9 + 1;
        constexpr static size_t LENGTH_D = 26 - 5 + 1;
        constexpr static size_t LENGTH_E = 26 - 5 + 1;

        /*! @brief  (A) Beacon */
        struct A
        {
            uint16_t _companyID;
            uint8_t _beaconType[2];
            uint8_t _uuid[16];
            uint16_t _mahor;
            uint16_t _minor;
            uint8_t _power;
        } __attribute__((__packed__));

        /*!
          @brief (B) Connection Advertise 1
          Scan Response (SCAN_RSP)
        */
        struct B
        {
            uint16_t _companyID;
            uint16_t _pageInformation;
            uint8_t _rowInformation;
            uint32_t _uniqueIdentifier;
            uint8_t _temperatureEvt;
            uint8_t _relativeHumidityEvt;
            uint8_t _ambientLightEvt;
            uint8_t _uvIndexEvt;
            uint8_t _presureEvt;
            uint8_t _soundNoiseEvt;
            uint8_t _discomfortIndexEvt;
            uint8_t _heatstrokeEvt;
            uint8_t _miscEvt;
            Temperature _temperature;
            RelativeHumidity _relativeHumidity;
            AmbientLight _ambientLight;
            Presure _presure;
            SoundNoise _soundNoise;
            BatteryVoltage8 _batteryVoltage;

            float temperature() const { return static_cast<float>(_temperature); }
            float relativeHumidity() const { return static_cast<float>(_relativeHumidity); }
            int16_t ambientLight() const { return static_cast<int16_t>(_ambientLight); }
            float presure() const { return static_cast<float>(_presure); }
            float soundNoise() const { return static_cast<float>(_soundNoise); }
            uint16_t batteryVoltage() const { return static_cast<uint16_t>(_batteryVoltage); }
        } __attribute__((__packed__));

        /*! @brief  (C) Connection Advertise 2 (ADV_IND) */
        struct C
        {
            uint16_t _companyID;
            uint16_t _pageRowInformation;
            uint32_t _uniqueIdentifier;
            uint8_t _temperatureEvt;
            uint8_t _relativeHumidityEvt;
            uint8_t _ambientLightEvt;
            uint8_t _uvIndexEvt;
            uint8_t _presureEvt;
            uint8_t _soundNoiseEvt;
            uint8_t _discomfortIndexEvt;
            uint8_t _heatstrokeEvt;
            uint8_t _miscEvt;

            uint16_t page() const { return _pageRowInformation >> 4; }
            uint8_t row() const { return _pageRowInformation & 0x0F; }
        } __attribute__((__packed__));

        /*! @brief (D) Sensor ADV 1 (ADV_IND) */
        struct D
        {
            uint16_t _companyID;
            uint8_t _sequence;
            Temperature _temperature;
            RelativeHumidity _relativeHumidity;
            AmbientLight _ambientLight;
            UVIndex _uvIndex;
            Presure _presure;
            SoundNoise _soundNoise;
            Acceleration _accelerationX;
            Acceleration _accelerationY;
            Acceleration _accelerationZ;
            BatteryVoltage8 _batteryVoltage;

            float temperature() const { return static_cast<float>(_temperature); }
            float relativeHumidity() const { return static_cast<float>(_relativeHumidity); }
            int16_t ambientLight() const { return static_cast<int16_t>(_ambientLight); }
            float uvIndex() const { return static_cast<float>(_uvIndex); }
            float presure() const { return static_cast<float>(_presure); }
            float soundNoise() const { return static_cast<float>(_soundNoise); }
            float accelerationX() const { return static_cast<float>(_accelerationX); }
            float accelerationY() const { return static_cast<float>(_accelerationY); }
            float accelerationZ() const { return static_cast<float>(_accelerationZ); }
            uint16_t batteryVoltage() const { return static_cast<uint16_t>(_batteryVoltage); }
        } __attribute__((__packed__));

        /*! @brief (E) Sensor ADV 2 (ADV_IND) */
        struct E
        {
            uint16_t _companyID;
            uint8_t _sequence;
            Temperature _temperature;
            RelativeHumidity _relativeHumidity;
            AmbientLight _ambientLight;
            UVIndex _uvIndex;
            Presure _presure;
            SoundNoise _soundNoise;
            DiscomfortIndex _discomfortIndex;
            Heatstroke _heatstroke;
            uint16_t _rfu;
            BatteryVoltage8 _batteryVoltage;

            float temperature() const { return static_cast<float>(_temperature); }
            float relativeHumidity() const { return static_cast<float>(_relativeHumidity); }
            int16_t ambientLight() const { return static_cast<int16_t>(_ambientLight); }
            float uvIndex() const { return static_cast<float>(_uvIndex); }
            float presure() const { return static_cast<float>(_presure); }
            float soundNoise() const { return static_cast<float>(_soundNoise); }
            float discomfortIndex() const { return static_cast<float>(_discomfortIndex); }
            float heatstroke() const { return static_cast<float>(_heatstroke); }
            uint16_t batteryVoltage() const { return static_cast<uint16_t>(_batteryVoltage); }
        } __attribute__((__packed__));

        AdvertiseData() : _format(ADVFormat::Unknown)
                        , _array{}
        {}

        ADVFormat format() const { return _format; }
        const A* getA() const { return _format == ADVFormat::A ? &_a : nullptr; }
        const B* getB() const { return _format == ADVFormat::B ? &_b : nullptr; }
        const C* getC() const { return _format == ADVFormat::C ? &_c : nullptr; }
        const D* getD() const { return _format == ADVFormat::D ? &_d : nullptr; }
        const E* getE() const { return _format == ADVFormat::E ? &_e : nullptr; }
        
        bool from(const BeaconMode bmode, const uint8_t* mdata, const size_t length);
        bool from(const ADVFormat fmt, const uint8_t* mdata, const size_t length);
        void clear() { _format = ADVFormat::Unknown; _array = {}; }

      private:
        ADVFormat _format;
        union
        {
            std::array<uint8_t,28>  _array;
            A _a;
            B _b;
            C _c;
            D _d;
            E _e;
        };
    };
};
#endif
