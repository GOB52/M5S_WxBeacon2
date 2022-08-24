/*!
  @file   wxbeacon2.cpp
  @brief  WxBeacon2 definitions [OMRON 2JCIE-BL01]
  @note hardware independent
*/
#include "wxbeacon2.hpp"
#include <algorithm>
#include <cstring>
#include <cassert>

namespace
{
template<class> struct is_stdarray :std::false_type {};
template<class T, std::size_t N>  struct is_stdarray<std::array<T, N>> : std::true_type {};

template<class T> bool string2array(T& a, const std::string& v)
{
    static_assert(is_stdarray<T>::value, "T must be std::array");

    a = {};
    if(a.empty() || v.empty() || v.length() == 0) { return false; }
    
    std::memcpy(a.data(), v.data(), std::min(a.size(), v.size()) );
    return v.size() == a.size();
}

bool toLocalTime(std::tm& t, time_t tt)
{
    t = std::tm{};
    std::tm* lt = std::localtime(&tt);
    if(lt) { t = *lt; }
    return lt;
}

//
}

//
const bool WxBeacon2::recordingMode[9] =
{
    true, true, false, false, false, false, false, true, true,
};

//
bool WxBeacon2::LatestData::from(const std::string& v)
{
    return string2array(_array, v);
}

//
bool WxBeacon2::LatestPage::localTime(std::tm& t) const
{
    time_t tt = _time32;
    return toLocalTime(t, tt);
}

bool WxBeacon2::LatestPage::from(const std::string& v)
{
    return string2array(_array, v);
}

//
bool WxBeacon2::RequestPage::from(const std::string& v)
{
    return string2array(_array, v);
}

//
bool WxBeacon2::ResponseFlag::from(const std::string& v)
{
    return string2array(_array, v);
}

//
bool WxBeacon2::ResponseData::from(const std::string& v)
{
    return string2array(_array, v);
}

//
bool WxBeacon2::MeasurementInterval::from(const std::string& v)
{
    return string2array(_array, v);
}

//
WxBeacon2::Data::Data(std::time_t pageTime, uint16_t interval, const WxBeacon2::ResponseData& rd)
        :  _time(pageTime + rd._data._row * interval)
        , _temperature(rd._data._temperature)
        , _relativeHumidity(rd._data._relativeHumidity)
        , _ambientLight(rd._data._ambientLight)
        , _uvIndex(rd._data._uvIndex)
        , _presure(rd._data._presure)
        , _soundNoise(rd._data._soundNoise)
        , _discomfortIndex(rd._data._discomfortIndex)
        , _heatstroke(rd._data._heatstroke)
        , _batteryVoltage(rd._data._batteryVoltage)
{}

bool WxBeacon2::Data::localTime(std::tm& t) const
{
    return toLocalTime(t, _time);
}

//
bool WxBeacon2::TimeInformation::from(const std::string& v)
{
    return string2array(_array, v);
}

//
bool WxBeacon2::ErrorStatus::from(const std::string& v)
{
    return string2array(_array, v);
}

// Default setting value
const WxBeacon2::ADVSetting WxBeacon2::ADVSetting::DEFAULT_SETTING =
{
    // little endian
    0x08, 0x08, 0xA0, 0x00, 0x0A, 0x00, 0x32, 0x00, 0x08, 0x00
};

//
bool WxBeacon2::ADVSetting::from(const std::string& v)
{
    return string2array(_array, v);
}

//
bool WxBeacon2::AdvertiseData::from(const WxBeacon2::BeaconMode bmode, const uint8_t* mdata, const size_t length)
{
    return from(_format = formatTable[static_cast<int>(bmode)], mdata, length);
}

bool WxBeacon2::AdvertiseData::from(const WxBeacon2::ADVFormat fmt, const uint8_t* mdata, const size_t length)
{
    clear();
    _format = fmt;
    if(mdata && _format != WxBeacon2::ADVFormat::Unknown)
    {
        std::memcpy(_array.data(), mdata, std::min(length, _array.size()) );
    }
    return (_format != WxBeacon2::ADVFormat::Unknown) && mdata && length;
}

// for doxygen
/*!
  @mainpage M5S WxBeacon2
  Access for WxBeacon2 (OEM OMRON 2JCIE-BL01) using BLE.
  using C++11 or later.

  @note depends on NimBLE-Arduino
  
  @sa https://weathernews.jp/smart/wxbeacon2/
  @sa https://www.fa.omron.co.jp/products/family/3723/

  @author GOB https://twitter.com/gob_52_gob
  @copyright 2022 GOB
  @copyright Licensed under the MIT license. See LICENSE file in the project root for full license information.
*/
