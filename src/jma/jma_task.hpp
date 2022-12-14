
#ifndef JMA_TASK_HPP
#define JMA_TASK_HPP

#include "jma_forecast.hpp"

namespace jma
{
using forecast_callback = void(*)(const officecode_t oc, const Forecast& fc, const WeeklyForecast& wfc);

bool initializeForecast(const UBaseType_t priority, const BaseType_t core, forecast_callback f);
void finalizeForecast();

bool requestForecast();
bool busyForecast();

}
#endif
