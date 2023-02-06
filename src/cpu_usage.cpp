
#include "cpu_usage.hpp"
#include <esp_freertos_hooks.h>

namespace
{
bool initialized = false;
long idle_cnt_0, idle_cnt_1, tick_cnt_0, tick_cnt_1;

bool IRAM_ATTR function_idle_0(){ ++idle_cnt_0; return true; }
void IRAM_ATTR function_tick_0(){ ++tick_cnt_0; }
bool IRAM_ATTR function_idle_1(){ ++idle_cnt_1; return true; }
void IRAM_ATTR function_tick_1(){ ++tick_cnt_1; }
//
}

namespace cpu_usage
{

bool initialize()
{
    if(initialized) { return false; }

    auto i0=  esp_register_freertos_idle_hook_for_cpu(function_idle_0, 0);
    auto t0 = esp_register_freertos_tick_hook_for_cpu(function_tick_0, 0);
    auto i1 = esp_register_freertos_idle_hook_for_cpu(function_idle_1, 1);
    auto t1 = esp_register_freertos_tick_hook_for_cpu(function_tick_1, 1);
    initialized = (i0 == ESP_OK) && (t0 == ESP_OK) && (i1 == ESP_OK) && (t1 == ESP_OK);
    return initialized;
}

void reset()
{
    idle_cnt_0 = idle_cnt_1 = tick_cnt_0 = tick_cnt_1 = 0;
}

float cpu0()
{
    return (tick_cnt_0 ? (idle_cnt_0 * 100.0f / tick_cnt_0) : 100.0f);
}

float cpu1()
{
    return (tick_cnt_1 ? (idle_cnt_1 * 100.0f / tick_cnt_1) : 100.0f);
}
//
}
