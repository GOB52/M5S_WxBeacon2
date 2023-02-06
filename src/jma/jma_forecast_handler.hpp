#ifndef JMA_FORECAST_PARSER_HPP
#define JMA_FORECAST_PARSER_HPP

#include <gob_json_element_path.hpp>
#include <gob_json_element_value.hpp>
#include <gob_json_delegate_handler.hpp>
#include "jma_forecast.hpp"

// JSON handler for JMA forecast 
class JMAHandler : public goblib::json::DelegateHandler
{
    using ElementPath = goblib::json::ElementPath;
    using ElementValue = goblib::json::ElementValue;

  public:
    class Area : public Delegater
    {
      public:
        Area(jma::Area& target) : _v(target) {}
        virtual void value(const ElementPath& path, const ElementValue& value) override;
      private:
        jma::Area& _v;
    };
    //
    class Forecast : public Delegater
    {
        //
        class TimeSeriesWeather : public Delegater
        {
            //
            class Areas : public Delegater
            {
              public:
                Areas(jma::Forecast::TimeSeriesWeather::Areas& target) : _v(target) {}
                virtual Delegater* startObject(const ElementPath& path) override;
                virtual void value(const ElementPath& path, const ElementValue& value) override;
              private:
                jma::Forecast::TimeSeriesWeather::Areas& _v;
            };
          public:
            TimeSeriesWeather(jma::Forecast::TimeSeriesWeather& target) : _v(target) {}
            virtual Delegater* startObject(const ElementPath& path) override;
            virtual void value(const ElementPath& path, const ElementValue& value) override;
          private:
            jma::Forecast::TimeSeriesWeather& _v;
        };
        //
        class TimeSeriesPop : public Delegater
        {
          public:
            class Areas : public Delegater
            {
              public:
                Areas(jma::Forecast::TimeSeriesPop::Areas& target) : _v(target) {}
                virtual Delegater* startObject(const ElementPath& path) override;
                virtual void value(const ElementPath& path, const ElementValue& value) override;
              private:
                jma::Forecast::TimeSeriesPop::Areas& _v;
            };
            TimeSeriesPop(jma::Forecast::TimeSeriesPop& target) : _v(target) {}
            virtual Delegater* startObject(const ElementPath& path) override;
            virtual void value(const ElementPath& path, const ElementValue& value) override;
          private:
            jma::Forecast::TimeSeriesPop& _v;
        };
        //
        class TimeSeriesTemp : public Delegater
        {
          public:
            class Areas : public Delegater
            {
              public:
                Areas(jma::Forecast::TimeSeriesTemp::Areas& target) : _v(target) {}
                virtual Delegater* startObject(const ElementPath& path) override;
                virtual void value(const ElementPath& path, const ElementValue& value) override;
              private:
                jma::Forecast::TimeSeriesTemp::Areas& _v;
            };
            TimeSeriesTemp(jma::Forecast::TimeSeriesTemp& target) : _v(target) {}
            virtual Delegater* startObject(const ElementPath& path) override;
            virtual void value(const ElementPath& path, const ElementValue& value) override;
          private:
            jma::Forecast::TimeSeriesTemp& _v;
        };

      public:
        Forecast(jma::Forecast& target) : _v(target) {}
        virtual Delegater* startObject(const ElementPath& path) override;
        virtual void value(const ElementPath& path, const ElementValue& value) override;
      private:
        jma::Forecast& _v;
    };

    //
    class WeeklyForecast : public Delegater
    {
        //
        class TimeSeriesWeather : public Delegater
        {
            //
            class Areas : public Delegater
            {
              public:
                Areas(jma::WeeklyForecast::TimeSeriesWeather::Areas& target) : _v(target) {}
                virtual Delegater* startObject(const ElementPath& path) override;
                virtual void value(const ElementPath& path, const ElementValue& value) override;
              private:
                jma::WeeklyForecast::TimeSeriesWeather::Areas& _v;
            };
          public:
            TimeSeriesWeather(jma::WeeklyForecast::TimeSeriesWeather& target) : _v(target) {}
            virtual Delegater* startObject(const ElementPath& path) override;
            virtual void value(const ElementPath& path, const ElementValue& value) override;
          private:
            jma::WeeklyForecast::TimeSeriesWeather& _v;
        };
        //
        class TimeSeriesTemp : public Delegater
        {
          public:
            class Areas : public Delegater
            {
              public:
                Areas(jma::WeeklyForecast::TimeSeriesTemp::Areas& target) : _v(target) {}
                virtual Delegater* startObject(const ElementPath& path) override;
                virtual void value(const ElementPath& path, const ElementValue& value) override;
              private:
                jma::WeeklyForecast::TimeSeriesTemp::Areas& _v;
            };
            TimeSeriesTemp(jma::WeeklyForecast::TimeSeriesTemp& target) : _v(target) {}
            virtual Delegater* startObject(const ElementPath& path) override;
            virtual void value(const ElementPath& path, const ElementValue& value) override;
          private:
            jma::WeeklyForecast::TimeSeriesTemp& _v;
        };
        //
        class TempAverage : public Delegater
        {
          public:
            class Areas : public Delegater
            {
              public:
                Areas(jma::WeeklyForecast::TempAverage::Areas& target) : _v(target) {}
                virtual Delegater* startObject(const ElementPath& path) override;
                virtual void value(const ElementPath& path, const ElementValue& value) override;
              private:
                jma::WeeklyForecast::TempAverage::Areas& _v;
            };
            TempAverage(jma::WeeklyForecast::TempAverage& target) : _v(target) {}
            virtual Delegater* startObject(const ElementPath& path) override;
            virtual void value(const ElementPath& path, const ElementValue& value) override;
          private:
            jma::WeeklyForecast::TempAverage& _v;
        };
        //
        class PrecipAverage : public Delegater
        {
          public:
            class Areas : public Delegater
            {
              public:
                Areas(jma::WeeklyForecast::PrecipAverage::Areas& target) : _v(target) {}
                virtual Delegater* startObject(const ElementPath& path) override;
                virtual void value(const ElementPath& path, const ElementValue& value) override;
              private:
                jma::WeeklyForecast::PrecipAverage::Areas& _v;
            };
            PrecipAverage(jma::WeeklyForecast::PrecipAverage& target) : _v(target) {}
            virtual Delegater* startObject(const ElementPath& path) override;
            virtual void value(const ElementPath& path, const ElementValue& value) override;
          private:
            jma::WeeklyForecast::PrecipAverage& _v;
        };

      public:
        WeeklyForecast(jma::WeeklyForecast& target) : _v(target) {}
        virtual Delegater* startObject(const ElementPath& path) override;
        virtual void value(const ElementPath& path, const ElementValue& value) override;
      private:
        jma::WeeklyForecast& _v;
    };
    
  public:
    void startObject(const ElementPath& path) override;

    jma::Forecast _forecast{};
    jma::WeeklyForecast _weeklyForecast{};
};

#if 0
void test_json();
#endif

#endif
