
#include "jma_forecast.hpp"
#include <type_traits>
#include <cstdio>
#include <numeric>

namespace 
{
// Exists T::toString()?
template<class T> struct has_toString
{                                              
    template<class U, int d = (&U::toString, 0)> static std::true_type check(U*);
    static std::false_type check(...);
    static T* _cls;
    static const bool value = decltype(check(_cls))::value;
};

// Like join function of Ruby. (For Integral, Floating-point, and String)
template<typename T, typename std::enable_if<std::is_floating_point<T>::value || std::is_integral<T>::value || std::is_same<T, String>::value, std::nullptr_t>::type = nullptr>
String join(const std::vector<T>& v, const char* separator = ", ")
{

    return !v.empty() ? std::accumulate(++v.cbegin(), v.cend(), String(v[0]),
                                        [&separator](const String& a, const T& b)
                                        {
                                            return a + separator + String(b);
                                        }): String();
}


// Like join function of Ruby. (For classes that has the toString function.)
template<typename T, typename std::enable_if<has_toString<T>::value, std::nullptr_t>::type = nullptr>
String join(const std::vector<T>& v, const char* separator = ", ")
{
    return !v.empty() ? std::accumulate(++v.cbegin(), v.cend(), v[0].toString(),
                                        [&separator](const String& a, const T& b)
                                        {
                                            return a + separator + b.toString();
                                        }) : String();
}

template <typename T> constexpr typename std::underlying_type<T>::type to_underlying(T e) noexcept
{
    return static_cast<typename std::underlying_type<T>::type>(e);
}
//
}

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
    return Reliability((v >= 0 && v < to_underlying(Rank::MAX)) ? (Rank)v : Rank::MAX);
}

String Reliability::toString() const
{
    static constexpr char tbl[] = { 'A', 'B', 'C', '-'};
    auto v = to_underlying(_rank);
    return String((v < to_underlying(Rank::MAX)) ? tbl[v] : tbl[to_underlying(Rank::MAX)]);
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
    s = "timeDefine:[" + join(_timeDefines) + "]\n";
    s += join(_areas, "\n");
    return s;
}

String Forecast::TimeSeriesWeather::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "weatherCodes:[" + join(_weatherCodes) + "]\n";
    s += "weathers:[" + join(_weathers) + "]\n";
    s += "winds:[" + join(_winds) + "]\n";
    s += "waves:[" + join(_waves) + "]";
    return s;
}

String Forecast::TimeSeriesPop::toString() const
{
    String s;
    s = "timeDefine:[" + join(_timeDefines) + "]\n";
    s += join(_areas, "\n");
    return s;
}

String Forecast::TimeSeriesPop::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "pops:[" + join(_pops) + "]";
    return s;
}

String Forecast::TimeSeriesTemp::toString() const
{
    String s;
    s = "timeDefine:[" + join(_timeDefines) + "]\n";
    s += join(_areas, "\n");
    return s;
}

String Forecast::TimeSeriesTemp::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "temps:[" + join(_temps) + "]";
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
    s = "timeDefine:[" + join(_timeDefines) + "]\n";
    s += join(_areas, "\n");
    return s;
}

String WeeklyForecast::TimeSeriesWeather::Areas::toString() const
{
    String s = _area.toString() + '\n';
    s += "weatherCodes:[" + join(_weatherCodes) + "]\n";
    s += "pops:[" + join(_pops) + "]\n";
    s += "reliabilities:[" + join(_reliabilities) + ']';
    return s;
}

String WeeklyForecast::TimeSeriesTemp::toString() const
{
    String s;
    s = "timeDefine:[" + join(_timeDefines) + "]\n";
    s += join(_areas, "\n");
    return s;
}

String WeeklyForecast::TimeSeriesTemp::Areas::toString() const
{
   String s = _area.toString() + '\n';
   s += "tempsMin:[" + join(_tempsMin) + "]\n";
   s += "tempsMinUpper[:" + join(_tempsMinUpper) + "]\n";
   s += "tempsMinLower:[" + join(_tempsMinLower) + "]\n";
   s += "tempsMax:[" + join(_tempsMax) + "]\n";
   s += "tempsMaxUpper:[" + join(_tempsMaxUpper) + "]\n";
   s += "tempsMaxLower:[" + join(_tempsMaxLower) + "]";
   return s;
}

String WeeklyForecast::TempAverage::toString() const
{
    String s = join(_areas, "\n");
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
    String s = join(_areas, "\n");
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
