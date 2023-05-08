/*
  Request image of Weather Satellite "Himawari"
  WARNING : Works only with devices that have PSRAM (Need huge memories)
*/
#ifndef HIMAWARI_TASK_HPP
#define HIMAWARI_TASK_HPP

#include "himawari.hpp"
#include <gob_datetime.hpp>

namespace himawari
{
using image_callback = void(*)(const uint8_t* p, const Band band, const goblib::datetime::OffsetDateTime& odt);
using progress_callback = void(*)(const size_t readed, const size_t size);

bool initialize(const UBaseType_t priority, const BaseType_t core, image_callback ic, progress_callback pc);
bool busy();

bool request(const goblib::datetime::OffsetDateTime& odt, const Area = Area::Japan, const Band = Band::B13); // odt with ZoneOffset

//
}
#endif
