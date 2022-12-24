#ifndef JMA_DEFINITION_HPP
#define JMA_DEFINITION_HPP

#include <cstdint>

namespace jma
{

using officecode_t = uint32_t;
using weathercode_t = uint16_t;

const char* weatherCodeToString(const weathercode_t wc);
const char* officesCodeToString(const officecode_t oc);
int16_t weatherCodeToIcon(const weathercode_t wc);

extern const char root_ca_JMA[];

//
}
#endif
