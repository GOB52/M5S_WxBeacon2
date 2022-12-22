/*
  Request image of Weather Satellite "Himawari"
  M5Stack Core2 only
*/
#ifndef HIMAWARI_HPP
#define HIMAWARI_HPP

# ifdef ARDUINO_M5STACK_Core2

namespace himawari
{
using image_callback = void(*)(const uint8_t* p);

bool initialize(const UBaseType_t priority, const BaseType_t core, image_callback f);
bool busy();
bool request();

//
}
# endif
#endif
