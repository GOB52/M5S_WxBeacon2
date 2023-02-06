
#include "jma_forecast.hpp"
#include "utility.hpp"
#include <type_traits>
#include <cstdio>
#include <numeric>

namespace jma
{
//
String Area::toString() const
{
    String s = "name:" + _name + ", code:" + String(_code);
    return s;
}

//
Reliability Reliability::parse(const char* s)
{
    if(!s || !s[0] || s[1]) { return Reliability(Rank::MAX); }
    int v = *s - 'A';
    return Reliability((v >= 0 && v < gob::to_underlying(Rank::MAX)) ? (Rank)v : Rank::MAX);
}

String Reliability::toString() const
{
    static constexpr char tbl[] = { 'A', 'B', 'C', '-'};
    auto v = gob::to_underlying(_rank);
    return String((v < gob::to_underlying(Rank::MAX)) ? tbl[v] : tbl[gob::to_underlying(Rank::MAX)]);
}

//
String Forecast::toString() const
{
    String s;
    s += "publishingOffice:" + _publishingOffice + '\n';
    s += "reportDatetime:" + _reportDatetime.toString() + '\n';
    s += "timeSeries[0]:Weather\n" + _tsWeather.toString() + '\n';
    s += "timeSeries[1]:Pop\n" + _tsPop.toString() + '\n';
    s += "timeSeries[2]:Temp\n" + _tsTemp.toString();
    return s;
}


String Forecast::TimeSeriesWeather::toString() const
{
    String s;
    s = "timeDefine:[" + gob::join(_timeDefines) + "]\n";
    s += gob::join(_areas, "\n");
    return s;
}

String Forecast::TimeSeriesWeather::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "weatherCodes:[" + gob::join(_weatherCodes) + "]\n";
    s += "weathers:[" + gob::join(_weathers) + "]\n";
    s += "winds:[" + gob::join(_winds) + "]\n";
    s += "waves:[" + gob::join(_waves) + "]";
    return s;
}

String Forecast::TimeSeriesPop::toString() const
{
    String s;
    s = "timeDefine:[" + gob::join(_timeDefines) + "]\n";
    s += gob::join(_areas, "\n");
    return s;
}

String Forecast::TimeSeriesPop::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "pops:[" + gob::join(_pops) + "]";
    return s;
}

String Forecast::TimeSeriesTemp::toString() const
{
    String s;
    s = "timeDefine:[" + gob::join(_timeDefines) + "]\n";
    s += gob::join(_areas, "\n");
    return s;
}

String Forecast::TimeSeriesTemp::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "temps:[" + gob::join(_temps) + "]";
    return s;
}

//
String WeeklyForecast::toString() const
{
    String s;
    s += "publishingOffice:" + _publishingOffice + '\n';
    s += "reportDatetime:" + _reportDatetime.toString() + '\n';
    s += "timeSeries[0]:Weather\n" + _tsWeather.toString() + '\n';
    s += "timeSeries[1]:Temp\n" + _tsTemp.toString() + '\n';
    s += "tempAverage\n" + _tempAverage.toString() + '\n';
    s += "precipAverage:\n" + _precipAverage.toString();
    return s;
}

String WeeklyForecast::TimeSeriesWeather::toString() const
{
    String s;
    s = "timeDefine:[" + gob::join(_timeDefines) + "]\n";
    s += gob::join(_areas, "\n");
    return s;
}

String WeeklyForecast::TimeSeriesWeather::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "weatherCodes:[" + gob::join(_weatherCodes) + "]\n";
    s += "pops:[" + gob::join(_pops) + "]\n";
    s += "reliabilities:[" + gob::join(_reliabilities) + ']';
    return s;
}

String WeeklyForecast::TimeSeriesTemp::toString() const
{
    String s;
    s = "timeDefine:[" + gob::join(_timeDefines) + "]\n";
    s += gob::join(_areas, "\n");
    return s;
}

String WeeklyForecast::TimeSeriesTemp::Areas::toString() const
{
   String s = _area.toString() + '\n';
   s += "tempsMin:[" + gob::join(_tempsMin) + "]\n";
   s += "tempsMinUpper[:" + gob::join(_tempsMinUpper) + "]\n";
   s += "tempsMinLower:[" + gob::join(_tempsMinLower) + "]\n";
   s += "tempsMax:[" + gob::join(_tempsMax) + "]\n";
   s += "tempsMaxUpper:[" + gob::join(_tempsMaxUpper) + "]\n";
   s += "tempsMaxLower:[" + gob::join(_tempsMaxLower) + "]";
   return s;
}

String WeeklyForecast::TempAverage::toString() const
{
    String s = gob::join(_areas, "\n");
    return s;
}

String WeeklyForecast::TempAverage::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "min:" + String(_min) + ", max:" + String(_max);
    return s;
}

String WeeklyForecast::PrecipAverage::toString() const
{
    String s = gob::join(_areas, "\n");
    return s;
}

String WeeklyForecast::PrecipAverage::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "min:" + String(_min) + ", max:" + String(_max);
    return s;
}
//
}
