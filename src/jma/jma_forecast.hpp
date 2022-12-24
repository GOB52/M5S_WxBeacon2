
#ifndef JMA_FORECAST_HPP
#define JMA_FORECAST_HPP

#include "jma_definition.hpp"
#include <vector>
#include <WString.h>
#include <gob_datetime.hpp>
using goblib::datetime::OffsetDateTime;

namespace jma
{

struct Area
{
    String   _name{};
    uint32_t _code{};
    String toString() const;
};

struct Reliability
{
    enum class Rank : uint8_t { A, B, C, MAX };

    explicit Reliability(const Rank r) : _rank(r) {}
    explicit Reliability(const char* s) { *this = parse(s); }
    static Reliability parse(const char* s);
    String toString() const;
  private:
    Rank _rank{Rank::C};
};

//
struct Forecast
{
    struct TimeSeriesWeather
    {
        struct Areas
        {
            Area _area{};
            std::vector<weathercode_t> _weatherCodes;
            std::vector<String> _weathers;
            std::vector<String> _winds;
            std::vector<String> _waves;
            String toString() const;
        };
        std::vector<OffsetDateTime> _timeDefines;
        std::vector<Areas> _areas;
        String toString() const;
    };
    struct TimeSeriesPop
    {
        struct Areas
        {
            Area _area{};
            std::vector<int8_t> _pops;
            String toString() const;
        };
        std::vector<OffsetDateTime> _timeDefines;
        std::vector<Areas> _areas;
        String toString() const;
    };
    struct TimeSeriesTemp
    {
        struct Areas
        {
            Area _area{};
            std::vector<int8_t> _temps; // Pair 2 elements. [low,high,low,high...]
            String toString() const;
        };
        std::vector<OffsetDateTime> _timeDefines; // Same as _temps length
        std::vector<Areas> _areas;
        String toString() const;
    };

    
    String _publishingOffice{};
    OffsetDateTime _reportDatetime{};
    TimeSeriesWeather _tsWeather{};
    TimeSeriesPop _tsPop{};
    TimeSeriesTemp _tsTemp{};

    String toString() const;

    //
    bool existsTopWeatherCodes() const { return !_tsWeather._areas.empty() && !_tsWeather._areas[0]._weatherCodes.empty(); }
    const std::vector<weathercode_t>& TopWeatherCodes() const { return _tsWeather._areas[0]._weatherCodes; }
    const std::vector<OffsetDateTime>& TopWeatherTime() const { return _tsWeather._timeDefines; }

    bool existsTopTemp() const { return !_tsTemp._areas.empty() && !_tsTemp._areas[0]._temps.empty(); }
    const std::vector<int8_t>& TopTemp() const { return _tsTemp._areas[0]._temps; }
    const std::vector<OffsetDateTime>& TopTempTime() const { return _tsTemp._timeDefines; }
};


// 週間予報
struct WeeklyForecast
{
    struct TimeSeriesWeather
    {
        struct Areas
        {
            Area _area;
            std::vector<weathercode_t> _weatherCodes;
            std::vector<int8_t> _pops;
            std::vector<Reliability> _reliabilities;
            String toString() const;
        };
        std::vector<OffsetDateTime> _timeDefines;
        std::vector<Areas> _areas;
        String toString() const;
    };

    struct TimeSeriesTemp
    {
        struct Areas
        {
            Area _area;
            std::vector<int8_t> _tempsMin;
            std::vector<int8_t> _tempsMinUpper;
            std::vector<int8_t> _tempsMinLower;
            std::vector<int8_t> _tempsMax;
            std::vector<int8_t> _tempsMaxUpper;
            std::vector<int8_t> _tempsMaxLower;
            String toString() const;
        };
        std::vector<OffsetDateTime> _timeDefines;
        std::vector<Areas> _areas;
        String toString() const;
    };
    struct TempAverage
    {
        struct Areas
        {
            Area _area{};
            float _min{};
            float _max{};
            String toString() const;
        };
        std::vector<Areas> _areas;
        String toString() const;
    };
    struct PrecipAverage
    {
        struct Areas
        {
            Area _area{};
            float _min{};
            float _max{};
            String toString() const;
        };
        std::vector<Areas> _areas;
        String toString() const;
    };
    
    String _publishingOffice;
    OffsetDateTime _reportDatetime{};
    TimeSeriesWeather _tsWeather{};
    TimeSeriesTemp _tsTemp{};
    TempAverage _tempAverage;
    PrecipAverage _precipAverage;
    String toString() const;
};

//
}
#endif
