
#ifndef JMA_TASK_HPP
#define JMA_TASK_HPP

#include "jma_forecast.hpp"

namespace jma
{
using forecast_callback = void(*)(const officecode_t oc, const Forecast& fc, const WeeklyForecast& wfc);
using progress_callback = void(*)(const size_t readed, const size_t size);

bool initializeForecast(const UBaseType_t priority, const BaseType_t core, forecast_callback fc, progress_callback pc);
void finalizeForecast();

bool requestForecast();
bool busyForecast();

}
#endif
