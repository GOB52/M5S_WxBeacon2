
#ifndef HIMAWARI_HPP
#define HIMAWARI_HPP

#include <WString.h>

namespace himawari
{
// Band type
enum class Band : uint8_t
{
    B13, // Infrared
    B03, // Visible
    B08, // Water Vapor
    B07, // Short Wave Infrared
    DayMicrophysicsRGB,
    NightMicrophysicsRGB,
    DustRGB,
    AirmassRGB,
    DaySnowFogRGB,
    NaturalColorRGB,
    TrueColorRGB, // Enhanced
    TrueColorReproductionImage,
    DayConvectiveStormRGB,
    Sandwich,
    B03CombinedWithB13,
    B03AndB13AtNight,
    HeavyRainfallPotentialAreas,
    Max
};
// Area
enum class Area : uint8_t
{
    FullDisk,
    Japan,
    Australia,
    NewZealand,
    Max
};

String bandToString(const Band band);

//
}
#endif
