
#include <gob_json.hpp>
#include "jma_forecast_handler.hpp"
#include "wb2/wxbeacon2_log.hpp"

using goblib::json::ElementPath;
using goblib::json::ElementValue;
using goblib::json::ElementBase;
using goblib::json::Element;
using Delegater = goblib::json::DelegateHandler::Delegater;
using goblib::datetime::OffsetDateTime;

// Custom helper for specific types.
template<typename T> struct CustomElement : public Element<T>
{
    CustomElement(const char*k, T* vp) : Element<T>(k, vp) {}
    virtual void store(const ElementValue& ev, const int index = -1) override { _store(ev, index); }

    // OffsetDateTime
    template<typename U = T,
             typename std::enable_if<std::is_same<U, OffsetDateTime>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int) { *(this->value) = OffsetDateTime::parse(ev.getString()); }
    // vector<OffsetDateTime>
    template<typename U = T,
             typename std::enable_if<goblib::json::is_std_vector<U>::value && std::is_same<typename U::value_type, OffsetDateTime>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int) { this->value->emplace_back(ev.getString()); }
    // vector<Reliability>
    template<typename U = T,
             typename std::enable_if<goblib::json::is_std_vector<U>::value && std::is_same<typename U::value_type, jma::Reliability>::value, std::nullptr_t>::type = nullptr>
    void _store(const ElementValue& ev, const int) { this->value->emplace_back(ev.getString()); }
};

// Handler
void JMAHandler::startObject(const ElementPath& path)
{
    //    WB2_LOGV("%s:index:%d count:%d", path.toString().c_str(), path.getIndex(), path.getCount());
    Delegater* obj = nullptr;
    // pushDelegater if top level object in top array.
    if(path.getCount() == 1)
    {
        switch(path.getIndex())
        {
        case 0: obj = new Forecast(_forecast); break;
        case 1: obj = new WeeklyForecast(_weeklyForecast); break;
        }
        if(obj) {  pushDelegater(obj); return; }
        WB2_LOGE("Missing object");
    }
    DelegateHandler::startObject(path);
}

// Area
void JMAHandler::Area::value(const  ElementPath& path, const ElementValue& value)
{
    Element<decltype(_v._name)> e_name { "name",  &_v._name };
    Element<decltype(_v._code)> e_code { "code",  &_v._code };
    ElementBase* elements[] = { &e_name, &e_code };
    //    WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == path.getKey()) { e->store(value); return; } }
}

// ForeeCast
Delegater* JMAHandler::Forecast::startObject(const ElementPath& path)
{
    //    WB2_LOGV("%s:index:%d count:%d", path.toString().c_str(), path.getIndex(), path.getCount());
    Delegater* obj = nullptr;
    switch(path.getIndex())
    {
    case 0: obj = new TimeSeriesWeather(_v._tsWeather); break;
    case 1: obj = new TimeSeriesPop(_v._tsPop); break;
    case 2: obj = new TimeSeriesTemp(_v._tsTemp); break;
    }
    return obj ? obj : Delegater::startObject(path);
}

void JMAHandler::Forecast::value(const ElementPath& path, const ElementValue& value)
{
    Element<decltype(_v._publishingOffice)>       e_publishingOffice { "publishingOffice",  &_v._publishingOffice };
    CustomElement<decltype(_v._reportDatetime)>   e_reportDatetime   { "reportDatetime",    &_v._reportDatetime   };
    ElementBase* elements[] = { &e_publishingOffice, &e_reportDatetime };
    WB2_LOGV("[%s] %d <%s>", path.toString().c_str(), value.isString(), value.getString());
    for(auto& e : elements) { if(*e == path.getKey()) { e->store(value); return; } }
    WB2_LOGE("oops!");
}

// Forecast::TimeSeriesWeather
Delegater* JMAHandler::Forecast::TimeSeriesWeather::startObject(const ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesWeather::Areas(_v._areas.back());
    }
    return Delegater::startObject(path);
}

void JMAHandler::Forecast::TimeSeriesWeather::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    CustomElement<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    //   WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

// Forecast::TimeSeriesWeather::Areas
Delegater* JMAHandler::Forecast::TimeSeriesWeather::Areas::startObject(const ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return Delegater::startObject(path);
}

void JMAHandler::Forecast::TimeSeriesWeather::Areas::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._weatherCodes)> e_codes    { "weatherCodes",  &_v._weatherCodes };
    //    Element<decltype(_v._weathers)>     e_weathers { "weathers",      &_v._weathers };
    //    Element<decltype(_v._winds)>        e_winds    { "winds",         &_v._winds };
    //    Element<decltype(_v._waves)>        e_waves    { "waves",         &_v._waves };
    //    ElementBase* elements[] = { &e_codes, &e_weathers, &e_winds, &e_waves };
    ElementBase* elements[] = { &e_codes };
    //   WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) {if(*e == parent->getKey()) { e->store(value); return; }}
}

// Forecast::TimeSeriesPop
Delegater* JMAHandler::Forecast::TimeSeriesPop::startObject(const ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesPop::Areas(_v._areas.back());
    }
    return Delegater::startObject(path);
}

void JMAHandler::Forecast::TimeSeriesPop::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    CustomElement<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    //   WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

// Forecast::TimeSeriesPop::Areas
Delegater* JMAHandler::Forecast::TimeSeriesPop::Areas::startObject(const ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return Delegater::startObject(path);
}

void JMAHandler::Forecast::TimeSeriesPop::Areas::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._pops)> e_pops    { "pops",  &_v._pops };
    ElementBase* elements[] = { &e_pops };
    //   WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

// Forecast::TimeSeriesTemp
Delegater* JMAHandler::Forecast::TimeSeriesTemp::startObject(const ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesTemp::Areas(_v._areas.back());
    }
    return Delegater::startObject(path);
}

void JMAHandler::Forecast::TimeSeriesTemp::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    CustomElement<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    //   WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

// Forecast::TimeSeriesTemp::Areas
Delegater* JMAHandler::Forecast::TimeSeriesTemp::Areas::startObject(const ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return Delegater::startObject(path);
}

void JMAHandler::Forecast::TimeSeriesTemp::Areas::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._temps)> e_temps    { "temps",  &_v._temps };
    ElementBase* elements[] = { &e_temps };
    //   WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

// WeeklyForeeCast
Delegater* JMAHandler::WeeklyForecast::startObject(const ElementPath& path)
{
    Delegater* obj = nullptr;
    auto parent = path.getParent();
    
    if(String("timeSeries") == parent->getKey())
    {
        switch(path.getIndex())
        {
        case 0: obj = new TimeSeriesWeather(_v._tsWeather); break;
        case 1: obj = new TimeSeriesTemp(_v._tsTemp); break;
        }
    }
    else if(String("tempAverage") == path.getKey())
    {
        obj = new TempAverage(_v._tempAverage);
    }
    else if(String("precipAverage") == path.getKey())
    {
        obj = new PrecipAverage(_v._precipAverage);
    }
    return obj ? obj : Delegater::startObject(path);
}

void JMAHandler::WeeklyForecast::value(const ElementPath& path, const ElementValue& value)
{
    Element<decltype(_v._publishingOffice)> e_publishingOffice { "publishingOffice",  &_v._publishingOffice };
    CustomElement<decltype(_v._reportDatetime)>   e_reportDatetime   { "reportDatetime",    &_v._reportDatetime   };
    ElementBase* elements[] = { &e_publishingOffice, &e_reportDatetime };
    //   WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == path.getKey()) { e->store(value); return; } }
}

// WeeklyForecast::TimeSeriesWeather
Delegater* JMAHandler::WeeklyForecast::TimeSeriesWeather::startObject(const ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesWeather::Areas(_v._areas.back());
    }
    return Delegater::startObject(path);
}

void JMAHandler::WeeklyForecast::TimeSeriesWeather::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    CustomElement<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    //   WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

// WeeklyForecast::TimeSeriesWeather::Areas
Delegater* JMAHandler::WeeklyForecast::TimeSeriesWeather::Areas::startObject(const ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return Delegater::startObject(path);
}

void JMAHandler::WeeklyForecast::TimeSeriesWeather::Areas::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._weatherCodes)>  e_codes         { "weatherCodes",  &_v._weatherCodes };
    Element<decltype(_v._pops)>          e_pops          { "pops",          &_v._pops };
    CustomElement<decltype(_v._reliabilities)> e_reliabilities { "reliabilities", &_v._reliabilities };
    ElementBase* elements[] = { &e_codes, &e_pops, &e_reliabilities };
    //    WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) {if(*e == parent->getKey()) { e->store(value); return; } }
}

// WeeklyForecast::TimeSeriesTemp
Delegater* JMAHandler::WeeklyForecast::TimeSeriesTemp::startObject(const ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesTemp::Areas(_v._areas.back());
    }
    return Delegater::startObject(path);
}

void JMAHandler::WeeklyForecast::TimeSeriesTemp::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    CustomElement<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    //    WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

// WeeklyForecast::TimeSeriesTemp::Areas
Delegater* JMAHandler::WeeklyForecast::TimeSeriesTemp::Areas::startObject(const ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return Delegater::startObject(path);
}

void JMAHandler::WeeklyForecast::TimeSeriesTemp::Areas::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._tempsMin)>       e_tempsMin  { "tempsMin",      &_v._tempsMin };
    Element<decltype(_v._tempsMinUpper)>  e_tempsMinU { "tempsMinUpper", &_v._tempsMinUpper };
    Element<decltype(_v._tempsMinLower)>  e_tempsMinL { "tempsMinLower", &_v._tempsMinLower };
    Element<decltype(_v._tempsMax)>       e_tempsMax  { "tempsMax",      &_v._tempsMax };
    Element<decltype(_v._tempsMaxUpper)>  e_tempsMaxU { "tempsMaxUpper", &_v._tempsMaxUpper };
    Element<decltype(_v._tempsMaxLower)>  e_tempsMaxL { "tempsMaxLower", &_v._tempsMaxLower };
    ElementBase* elements[] = { &e_tempsMin, &e_tempsMinU, &e_tempsMinL, &e_tempsMax, &e_tempsMaxU, &e_tempsMaxL };
    //    WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

// WeeklyForecast::TempAverage
Delegater* JMAHandler::WeeklyForecast::TempAverage::startObject(const ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TempAverage::Areas(_v._areas.back());
    }
    return Delegater::startObject(path);
}

void JMAHandler::WeeklyForecast::TempAverage::value(const ElementPath& path, const ElementValue& value)
{
    // No value is obtained.
}

// WeeklyForecast::TempAverage::Areas
Delegater* JMAHandler::WeeklyForecast::TempAverage::Areas::startObject(const ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return Delegater::startObject(path);
}

void JMAHandler::WeeklyForecast::TempAverage::Areas::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._min)>  e_min { "min", &_v._min };
    Element<decltype(_v._max)>  e_max { "max", &_v._max };
    ElementBase* elements[] = { &e_min, &e_max };
    //    WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

// WeeklyForecast::PrecipAverage
Delegater* JMAHandler::WeeklyForecast::PrecipAverage::startObject(const ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new PrecipAverage::Areas(_v._areas.back());
    }
    return Delegater::startObject(path);
}

void JMAHandler::WeeklyForecast::PrecipAverage::value(const ElementPath& path, const ElementValue& value)
{
}

// WeeklyForecast::PrecipAverage::Areas
Delegater* JMAHandler::WeeklyForecast::PrecipAverage::Areas::startObject(const ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return Delegater::startObject(path);
}

void JMAHandler::WeeklyForecast::PrecipAverage::Areas::value(const ElementPath& path, const ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._min)>  e_min { "min", &_v._min };
    Element<decltype(_v._max)>  e_max { "max", &_v._max };
    ElementBase* elements[] = { &e_min, &e_max };
    //    WB2_LOGV("[%s] <%s>", path.toString().c_str(), value.toString().c_str());
    for(auto& e : elements) { if(*e == parent->getKey()) { e->store(value); return; } }
}

#if 0
namespace
{
const char jma_json[] =
R"***(
[{"publishingOffice":"金沢地方気象台","reportDatetime":"2023-01-10T11:00:00+09:00","timeSeries":[{"timeDefines":["2023-01-10T11:00:00+09:00","2023-01-11T00:00:00+09:00","2023-01-12T00:00:00+09:00"],"areas":[{"area":{"name":"加賀","code":"170010"},"weatherCodes":["200","101","101"],"weathers":["くもり　所により　夕方　まで　雨か雪","晴れ　昼前　から　夕方　くもり","晴れ　時々　くもり"],"winds":["北の風　海上　では　北西の風　やや強く","南の風　海上　では　南西の風　やや強く","南の風　後　南西の風"],"waves":["３メートル　うねり　を伴う","２メートル　後　１．５メートル","１．５メートル　後　１メートル"]},{"area":{"name":"能登","code":"170020"},"weatherCodes":["200","111","201"],"weathers":["くもり　所により　夕方　まで　雨か雪","晴れ　昼前　から　くもり","くもり　時々　晴れ"],"winds":["北西の風　やや強く　後　西の風","南の風　海上　では　南西の風　やや強く","南の風"],"waves":["４メートル　後　３メートル　うねり　を伴う","２．５メートル　後　２メートル","２メートル"]}]},{"timeDefines":["2023-01-10T12:00:00+09:00","2023-01-10T18:00:00+09:00","2023-01-11T00:00:00+09:00","2023-01-11T06:00:00+09:00","2023-01-11T12:00:00+09:00","2023-01-11T18:00:00+09:00"],"areas":[{"area":{"name":"加賀","code":"170010"},"pops":["30","10","0","10","10","0"]},{"area":{"name":"能登","code":"170020"},"pops":["30","10","10","20","20","10"]}]},{"timeDefines":["2023-01-10T09:00:00+09:00","2023-01-10T00:00:00+09:00","2023-01-11T00:00:00+09:00","2023-01-11T09:00:00+09:00"],"areas":[{"area":{"name":"金沢","code":"56227"},"temps":["6","6","0","9"]},{"area":{"name":"輪島","code":"56052"},"temps":["4","5","0","9"]}]}]},{"publishingOffice":"金沢地方気象台","reportDatetime":"2023-01-10T11:00:00+09:00","timeSeries":[{"timeDefines":["2023-01-11T00:00:00+09:00","2023-01-12T00:00:00+09:00","2023-01-13T00:00:00+09:00","2023-01-14T00:00:00+09:00","2023-01-15T00:00:00+09:00","2023-01-16T00:00:00+09:00","2023-01-17T00:00:00+09:00"],"areas":[{"area":{"name":"石川県","code":"170000"},"weatherCodes":["101","101","201","203","202","206","206"],"pops":["","20","40","70","60","50","50"],"reliabilities":["","","C","A","B","C","C"]}]},{"timeDefines":["2023-01-11T00:00:00+09:00","2023-01-12T00:00:00+09:00","2023-01-13T00:00:00+09:00","2023-01-14T00:00:00+09:00","2023-01-15T00:00:00+09:00","2023-01-16T00:00:00+09:00","2023-01-17T00:00:00+09:00"],"areas":[{"area":{"name":"金沢","code":"56227"},"tempsMin":["","2","4","7","4","3","4"],"tempsMinUpper":["","5","5","9","6","5","6"],"tempsMinLower":["","1","2","3","1","1","2"],"tempsMax":["","14","16","16","9","8","8"],"tempsMaxUpper":["","16","18","18","13","11","11"],"tempsMaxLower":["","13","15","14","7","6","6"]}]}],"tempAverage":{"areas":[{"area":{"name":"金沢","code":"56227"},"min":"1.2","max":"7.1"}]},"precipAverage":{"areas":[{"area":{"name":"金沢","code":"56227"},"min":"39.9","max":"67.2"}]}}]
)***";
//
}
void test_json()
{
    JMAHandler handler;
    goblib::json::StreamingParser parser(&handler);
    for(auto& e : jma_json) { parser.parse(e); }
    WB2_LOGD("%s", handler._forecast.toString().c_str());
    WB2_LOGD("%s", handler._weeklyForecast.toString().c_str());
    
}
#endif
