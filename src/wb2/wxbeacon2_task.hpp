
#ifndef WXBEACON2_TASK_HPP
#define WXBEACON2_TASK_HPP

#include "wxbeacon2.hpp"

// callback function on get advertise data.
using advertise_callback = void(*)(const bool exists, const WxBeacon2::AdvertiseData& ad);

bool initilizeAdvertise(const UBaseType_t priority, const BaseType_t core, advertise_callback f);

bool busyAdvertise();

bool requestAdvertise();
bool turnOnLED();
bool changeBeaconMode();
bool changeDefaultSetting();

#endif
