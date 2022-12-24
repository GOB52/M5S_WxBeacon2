#ifndef JMA_FORECAST_PARSER_HPP
#define JMA_FORECAST_PARSER_HPP

#include <JsonHandler.h>
#include "jma_forecast.hpp"
#include <type_traits>
#include <stack>


template <class T> struct is_std_vector { static const bool value=false; };
template <class T> struct is_std_vector<std::vector<T> > { static const bool value=true; };

// Store functions for each type. (SFINAE)
struct ElementBase
{
    ElementBase(const char* s) : key(s) {}
    virtual void store(ElementValue& s) = 0;
    String key;
};

template<typename T> struct Element : public ElementBase
{
    T* value; // Pointer of store target.
    Element(const char* key, T* p) : ElementBase(key), value(p) { assert(p); }
    virtual void store(ElementValue& ev) override { _store(ev); } // _store function is called based on the type of T. (SFINAE)

    // Integral
    template<typename U = T,
             typename std::enable_if<!std::is_same<U, bool>::value && std::is_integral<U>::value, std::nullptr_t>::type = nullptr>
    void _store(ElementValue& ev) { *value = ev.isInt() ? ev.getInt() : String(ev.getString()).toInt(); }
    // Floating-point
    template<typename U = T,
             typename std::enable_if<std::is_floating_point<U>::value, std::nullptr_t>::type = nullptr>
    void _store(ElementValue& ev) { *value = ev.isFloat() ? ev.getFloat() : String(ev.getString()).toFloat(); }
    // String
    template<typename U = T,
             typename std::enable_if<std::is_same<U, String>::value, std::nullptr_t>::type = nullptr>
    void _store(ElementValue& ev) { *value = ev.getString(); }
    // Boolean
    template<typename U = T,
             typename std::enable_if<std::is_same<U, bool>::value, std::nullptr_t>::type = nullptr>
    void _store(ElementValue& ev) { *value = ev.isBool() ? ev.getBool() : (String("true") == ev.getString()); }
    // OffsetDateTime
    template<typename U = T,
             typename std::enable_if<std::is_same<U, OffsetDateTime>::value, std::nullptr_t>::type = nullptr>
    void _store(ElementValue& ev) { *value = OffsetDateTime::parse(ev.getString()); }
    // vector<Integral>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value && std::is_integral<typename U::value_type>::value, std::nullptr_t>::type = nullptr>
    void _store(ElementValue& ev) { value->emplace_back(ev.isInt() ? ev.getInt() : String(ev.getString()).toInt()); }
    // vector<string>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value && std::is_same<typename U::value_type, String>::value, std::nullptr_t>::type = nullptr>
    void _store(ElementValue& ev) { value->emplace_back(ev.getString()); }
    // vector<OffsetDateTime>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value && std::is_same<typename U::value_type, OffsetDateTime>::value, std::nullptr_t>::type = nullptr>
    void _store(ElementValue& ev) { value->emplace_back(ev.getString()); }
    // vector<Reliability>
    template<typename U = T,
             typename std::enable_if<is_std_vector<U>::value && std::is_same<typename U::value_type, jma::Reliability>::value, std::nullptr_t>::type = nullptr>
    void _store(ElementValue& ev) { value->emplace_back(ev.getString()); }
};

//
class JMAHandler : public JsonHandler
{
  public:
    struct Object // interface class.
    {
        virtual ~Object(){}
        virtual Object* startObject(ElementPath& path) { return new Object(); }
        virtual void endObject(ElementPath& path) {}
        virtual void startArray(ElementPath& path) {}
        virtual void endArray(ElementPath& path) {}
        virtual void value(ElementPath& path, ElementValue& value) {}
    };
    //
    class Area : public Object
    {
      public:
        Area(jma::Area& target) : _v(target) {}
        virtual void value(ElementPath& path, ElementValue& value) override;
      private:
        jma::Area& _v;
    };
    //
    class Forecast : public Object
    {
        //
        class TimeSeriesWeather : public Object
        {
            //
            class Areas : public Object
            {
              public:
                Areas(jma::Forecast::TimeSeriesWeather::Areas& target) : _v(target) {}
                virtual Object* startObject(ElementPath& path) override;
                virtual void value(ElementPath& path, ElementValue& value) override;
              private:
                jma::Forecast::TimeSeriesWeather::Areas& _v;
            };
          public:
            TimeSeriesWeather(jma::Forecast::TimeSeriesWeather& target) : _v(target) {}
            virtual Object* startObject(ElementPath& path) override;
            virtual void value(ElementPath& path, ElementValue& value) override;
          private:
            jma::Forecast::TimeSeriesWeather& _v;
        };
        //
        class TimeSeriesPop : public Object
        {
          public:
            class Areas : public Object
            {
              public:
                Areas(jma::Forecast::TimeSeriesPop::Areas& target) : _v(target) {}
                virtual Object* startObject(ElementPath& path) override;
                virtual void value(ElementPath& path, ElementValue& value) override;
              private:
                jma::Forecast::TimeSeriesPop::Areas& _v;
            };
            TimeSeriesPop(jma::Forecast::TimeSeriesPop& target) : _v(target) {}
            virtual Object* startObject(ElementPath& path) override;
            virtual void value(ElementPath& path, ElementValue& value) override;
          private:
            jma::Forecast::TimeSeriesPop& _v;
        };
        //
        class TimeSeriesTemp : public Object
        {
          public:
            class Areas : public Object
            {
              public:
                Areas(jma::Forecast::TimeSeriesTemp::Areas& target) : _v(target) {}
                virtual Object* startObject(ElementPath& path) override;
                virtual void value(ElementPath& path, ElementValue& value) override;
              private:
                jma::Forecast::TimeSeriesTemp::Areas& _v;
            };
            TimeSeriesTemp(jma::Forecast::TimeSeriesTemp& target) : _v(target) {}
            virtual Object* startObject(ElementPath& path) override;
            virtual void value(ElementPath& path, ElementValue& value) override;
          private:
            jma::Forecast::TimeSeriesTemp& _v;
        };

      public:
        Forecast(jma::Forecast& target) : _v(target) {}
        virtual Object* startObject(ElementPath& path) override;
        virtual void value(ElementPath& path, ElementValue& value) override;
      private:
        jma::Forecast& _v;
    };

    //
    class WeeklyForecast : public Object
    {
        //
        class TimeSeriesWeather : public Object
        {
            //
            class Areas : public Object
            {
              public:
                Areas(jma::WeeklyForecast::TimeSeriesWeather::Areas& target) : _v(target) {}
                virtual Object* startObject(ElementPath& path) override;
                virtual void value(ElementPath& path, ElementValue& value) override;
              private:
                jma::WeeklyForecast::TimeSeriesWeather::Areas& _v;
            };
          public:
            TimeSeriesWeather(jma::WeeklyForecast::TimeSeriesWeather& target) : _v(target) {}
            virtual Object* startObject(ElementPath& path) override;
            virtual void value(ElementPath& path, ElementValue& value) override;
          private:
            jma::WeeklyForecast::TimeSeriesWeather& _v;
        };
        //
        class TimeSeriesTemp : public Object
        {
          public:
            class Areas : public Object
            {
              public:
                Areas(jma::WeeklyForecast::TimeSeriesTemp::Areas& target) : _v(target) {}
                virtual Object* startObject(ElementPath& path) override;
                virtual void value(ElementPath& path, ElementValue& value) override;
              private:
                jma::WeeklyForecast::TimeSeriesTemp::Areas& _v;
            };
            TimeSeriesTemp(jma::WeeklyForecast::TimeSeriesTemp& target) : _v(target) {}
            virtual Object* startObject(ElementPath& path) override;
            virtual void value(ElementPath& path, ElementValue& value) override;
          private:
            jma::WeeklyForecast::TimeSeriesTemp& _v;
        };
        //
        class TempAverage : public Object
        {
          public:
            class Areas : public Object
            {
              public:
                Areas(jma::WeeklyForecast::TempAverage::Areas& target) : _v(target) {}
                virtual Object* startObject(ElementPath& path) override;
                virtual void value(ElementPath& path, ElementValue& value) override;
              private:
                jma::WeeklyForecast::TempAverage::Areas& _v;
            };
            TempAverage(jma::WeeklyForecast::TempAverage& target) : _v(target) {}
            virtual Object* startObject(ElementPath& path) override;
            virtual void value(ElementPath& path, ElementValue& value) override;
          private:
            jma::WeeklyForecast::TempAverage& _v;
        };
        //
        class PrecipAverage : public Object
        {
          public:
            class Areas : public Object
            {
              public:
                Areas(jma::WeeklyForecast::PrecipAverage::Areas& target) : _v(target) {}
                virtual Object* startObject(ElementPath& path) override;
                virtual void value(ElementPath& path, ElementValue& value) override;
              private:
                jma::WeeklyForecast::PrecipAverage::Areas& _v;
            };
            PrecipAverage(jma::WeeklyForecast::PrecipAverage& target) : _v(target) {}
            virtual Object* startObject(ElementPath& path) override;
            virtual void value(ElementPath& path, ElementValue& value) override;
          private:
            jma::WeeklyForecast::PrecipAverage& _v;
        };

      public:
        WeeklyForecast(jma::WeeklyForecast& target) : _v(target) {}
        virtual Object* startObject(ElementPath& path) override;
        virtual void value(ElementPath& path, ElementValue& value) override;
      private:
        jma::WeeklyForecast& _v;
    };
    
  public:
    // JsonHandler overrides
    void startDocument() override {}
    void endDocument()   override;
    void startObject(ElementPath path) override;
    void endObject(ElementPath path) override;
    void startArray(ElementPath path) override;
    void endArray(ElementPath path) override;
    void whitespace(char /*c*/) override {}
    void value(ElementPath path, ElementValue value) override;

    jma::Forecast _forecast{};
    jma::WeeklyForecast _weeklyForecast{};

  private:
    Object* currentObject() const { return !_objects.empty() ? _objects.top() : nullptr; }
    std::stack<Object*> _objects;
};


//void test_json();
#endif
