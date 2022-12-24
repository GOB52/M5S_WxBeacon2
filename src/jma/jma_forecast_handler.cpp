
#include <JsonStreamingParser2.h>
#include "jma_forecast_handler.hpp"
#include "wb2/wxbeacon2_log.hpp"

namespace
{
#ifndef NDEBUG
String fullpath(ElementPath& path)
{
    char buf[256] = "";
    path.toString(buf);
    return String(buf);
}
#endif
//
}

void JMAHandler::startObject(ElementPath path)
{
    // Delegate processing to object.
    auto obj = currentObject();
    if(obj)
    {
        auto nobj = obj->startObject(path);
        if(!nobj) { WB2_LOGD("NullObj:%s", fullpath(path).c_str()); }
        assert(nobj);
        _objects.push(nobj);
        return;
    }

    Object* nobj = nullptr;
    switch(path.getIndex())
    {
    case 0: nobj = new Forecast(_forecast); break;
    case 1: nobj = new WeeklyForecast(_weeklyForecast); break;
    }
    if(!nobj) { WB2_LOGD("NullObj:%s", fullpath(path).c_str()); nobj = new Object(); }
    _objects.push(nobj);
}

void JMAHandler::endObject(ElementPath path)
{
    // Delegate processing to object.
    auto obj = currentObject();
    if(obj)
    {
        obj->endObject(path);
        delete _objects.top();
        _objects.pop();
    }
}

void JMAHandler::startArray(ElementPath path)
{
    // Delegate processing to object.
    auto obj = currentObject();
    if(obj) { obj->startArray(path); }
}

void JMAHandler::endArray(ElementPath path)
{
    // Delegate processing to object.
    auto obj = currentObject();
    if(obj) { obj->endArray(path); }
}

void JMAHandler::value(ElementPath path, ElementValue value)
{
    // Delegate processing to object.
    auto obj = currentObject();
    if(obj) { obj->value(path, value); }
}

void JMAHandler::endDocument()
{
    //    WB2_LOGD("End of Document: objects %zu", _objects.size());
}

// Area
void JMAHandler::Area::value(ElementPath& path, ElementValue& value)
{
    Element<decltype(_v._name)> e_name { "name",  &_v._name };
    Element<decltype(_v._code)> e_code { "code",  &_v._code };
    ElementBase* elements[] = { &e_name, &e_code };
    for(auto& e : elements)
    {
        if(e->key == path.getKey()) { e->store(value); return; }

    }
}

// ForeeCast
JMAHandler::Object* JMAHandler::Forecast::startObject(ElementPath& path)
{
    Object* obj = nullptr;
    switch(path.getIndex())
    {
    case 0: obj = new TimeSeriesWeather(_v._tsWeather); break;
    case 1: obj = new TimeSeriesPop(_v._tsPop); break;
    case 2: obj = new TimeSeriesTemp(_v._tsTemp); break;
    }
    return obj ? obj : new Object();
}

void JMAHandler::Forecast::value(ElementPath& path, ElementValue& value)
{
    Element<decltype(_v._publishingOffice)> e_publishingOffice { "publishingOffice",  &_v._publishingOffice };
    Element<decltype(_v._reportDatetime)>   e_reportDatetime   { "reportDatetime",    &_v._reportDatetime   };
    ElementBase* elements[] = { &e_publishingOffice, &e_reportDatetime };
    for(auto& e : elements)
    {
        if(e->key == path.getKey()) { e->store(value); return; }

    }
}

// Forecast::TimeSeriesWeather
JMAHandler::Object* JMAHandler::Forecast::TimeSeriesWeather::startObject(ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesWeather::Areas(_v._areas.back());
    }
    return new Object();
}

void JMAHandler::Forecast::TimeSeriesWeather::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    Element<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// Forecast::TimeSeriesWeather::Areas
JMAHandler::Object* JMAHandler::Forecast::TimeSeriesWeather::Areas::startObject(ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return new Object();
}

void JMAHandler::Forecast::TimeSeriesWeather::Areas::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._weatherCodes)> e_codes    { "weatherCodes",  &_v._weatherCodes };
    //    Element<decltype(_v._weathers)>     e_weathers { "weathers",      &_v._weathers };
    //    Element<decltype(_v._winds)>        e_winds    { "winds",         &_v._winds };
    //    Element<decltype(_v._waves)>        e_waves    { "waves",         &_v._waves };
    //    ElementBase* elements[] = { &e_codes, &e_weathers, &e_winds, &e_waves };
    ElementBase* elements[] = { &e_codes };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// Forecast::TimeSeriesPop
JMAHandler::Object* JMAHandler::Forecast::TimeSeriesPop::startObject(ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesPop::Areas(_v._areas.back());
    }
    return new Object();
}

void JMAHandler::Forecast::TimeSeriesPop::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    Element<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// Forecast::TimeSeriesPop::Areas
JMAHandler::Object* JMAHandler::Forecast::TimeSeriesPop::Areas::startObject(ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return new Object();
}

void JMAHandler::Forecast::TimeSeriesPop::Areas::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._pops)> e_pops    { "pops",  &_v._pops };
    ElementBase* elements[] = { &e_pops };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// Forecast::TimeSeriesTemp
JMAHandler::Object* JMAHandler::Forecast::TimeSeriesTemp::startObject(ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesTemp::Areas(_v._areas.back());
    }
    return new Object();
}

void JMAHandler::Forecast::TimeSeriesTemp::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    Element<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// Forecast::TimeSeriesTemp::Areas
JMAHandler::Object* JMAHandler::Forecast::TimeSeriesTemp::Areas::startObject(ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return new Object();
}

void JMAHandler::Forecast::TimeSeriesTemp::Areas::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._temps)> e_temps    { "temps",  &_v._temps };
    ElementBase* elements[] = { &e_temps };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// WeeklyForeeCast
JMAHandler::Object* JMAHandler::WeeklyForecast::startObject(ElementPath& path)
{
    Object* obj = nullptr;
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
    return obj ? obj : new Object();
}

void JMAHandler::WeeklyForecast::value(ElementPath& path, ElementValue& value)
{
    Element<decltype(_v._publishingOffice)> e_publishingOffice { "publishingOffice",  &_v._publishingOffice };
    Element<decltype(_v._reportDatetime)>   e_reportDatetime   { "reportDatetime",    &_v._reportDatetime   };
    ElementBase* elements[] = { &e_publishingOffice, &e_reportDatetime };
    for(auto& e : elements)
    {
        if(e->key == path.getKey()) { e->store(value); return; }
    }
}

// WeeklyForecast::TimeSeriesWeather
JMAHandler::Object* JMAHandler::WeeklyForecast::TimeSeriesWeather::startObject(ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesWeather::Areas(_v._areas.back());
    }
    return new Object();
}

void JMAHandler::WeeklyForecast::TimeSeriesWeather::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    Element<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// WeeklyForecast::TimeSeriesWeather::Areas
JMAHandler::Object* JMAHandler::WeeklyForecast::TimeSeriesWeather::Areas::startObject(ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return new Object();
}

void JMAHandler::WeeklyForecast::TimeSeriesWeather::Areas::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._weatherCodes)>  e_codes         { "weatherCodes",  &_v._weatherCodes };
    Element<decltype(_v._pops)>          e_pops          { "pops",          &_v._pops };
    Element<decltype(_v._reliabilities)> e_reliabilities { "reliabilities", &_v._reliabilities };
    ElementBase* elements[] = { &e_codes, &e_pops, &e_reliabilities };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// WeeklyForecast::TimeSeriesTemp
JMAHandler::Object* JMAHandler::WeeklyForecast::TimeSeriesTemp::startObject(ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TimeSeriesTemp::Areas(_v._areas.back());
    }
    return new Object();
}

void JMAHandler::WeeklyForecast::TimeSeriesTemp::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    Element<decltype(_v._timeDefines)> e_td { "timeDefines",  &_v._timeDefines };
    ElementBase* elements[] = { &e_td };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// WeeklyForecast::TimeSeriesTemp::Areas
JMAHandler::Object* JMAHandler::WeeklyForecast::TimeSeriesTemp::Areas::startObject(ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return new Object();
}

void JMAHandler::WeeklyForecast::TimeSeriesTemp::Areas::value(ElementPath& path, ElementValue& value)
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
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// WeeklyForecast::TempAverage
JMAHandler::Object* JMAHandler::WeeklyForecast::TempAverage::startObject(ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new TempAverage::Areas(_v._areas.back());
    }
    return new Object();
}

void JMAHandler::WeeklyForecast::TempAverage::value(ElementPath& path, ElementValue& value)
{
}

// WeeklyForecast::TempAverage::Areas
JMAHandler::Object* JMAHandler::WeeklyForecast::TempAverage::Areas::startObject(ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return new Object();
}

void JMAHandler::WeeklyForecast::TempAverage::Areas::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._min)>  e_min { "min", &_v._min };
    Element<decltype(_v._max)>  e_max { "max", &_v._max };
    ElementBase* elements[] = { &e_min, &e_max };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

// WeeklyForecast::PrecipAverage
JMAHandler::Object* JMAHandler::WeeklyForecast::PrecipAverage::startObject(ElementPath& path)
{
    auto parent = path.getParent();
    if(parent && String("areas") == parent->getKey())
    {
        _v._areas.emplace_back();
        return new PrecipAverage::Areas(_v._areas.back());
    }
    return new Object();
}

void JMAHandler::WeeklyForecast::PrecipAverage::value(ElementPath& path, ElementValue& value)
{
}

// WeeklyForecast::PrecipAverage::Areas
JMAHandler::Object* JMAHandler::WeeklyForecast::PrecipAverage::Areas::startObject(ElementPath& path)
{
    if(String("area") == path.getKey())
    {
        return new JMAHandler::Area(_v._area);
    }
    return new Object();
}

void JMAHandler::WeeklyForecast::PrecipAverage::Areas::value(ElementPath& path, ElementValue& value)
{
    auto parent = path.getParent();
    if(!parent) { return; }
    
    Element<decltype(_v._min)>  e_min { "min", &_v._min };
    Element<decltype(_v._max)>  e_max { "max", &_v._max };
    ElementBase* elements[] = { &e_min, &e_max };
    for(auto& e : elements)
    {
        if(e->key == parent->getKey()) { e->store(value); return; }

    }
}

#if 0
void test_json()
{
    auto mem = esp_get_free_heap_size();
    WB2_LOGD("before:%u", mem);

    {    
        JsonStreamingParser parser;
        JMAHandler handler;
        parser.setHandler(&handler);


        for(auto& e : test)
        {
            parser.parse(e);
        }
        printf("--------------------\n");
        printf("%s\n", handler._forecast.toString().c_str());
        printf("--------------------\n");
        printf("%s\n", handler._weeklyForecast.toString().c_str());
    }

    auto mem2 = esp_get_free_heap_size();
    WB2_LOGD("after:%u diff:%d", mem2, mem2 - mem);

    {    
        JsonStreamingParser parser;
        JMAHandler handler;
        parser.setHandler(&handler);
    }
    auto mem3 = esp_get_free_heap_size();
    WB2_LOGD("after:%u diff:%d", mem3, mem3 - mem2);
}
#endif
