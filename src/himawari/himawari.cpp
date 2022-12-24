

#include "himawari.hpp"
#include "utility.hpp"

namespace
{
PROGMEM const char band_B13[] = "B13 (Infrared)";
PROGMEM const char band_B03[] = "B03 (Visible)";
PROGMEM const char band_B08[] = "B08 (Water Vapor)";
PROGMEM const char band_B07[] = "B07 (Short Wave Infrared)";
PROGMEM const char band_DayMicrophysicsRGB[] = "Day Microphysics RGG";
PROGMEM const char band_NightMicrophysicsRGB[] = "Night Microphysics RGB";
PROGMEM const char band_DustRGB[] = "Dust RGB";
PROGMEM const char band_AirmassRGB[] = "Airimass RGB";
PROGMEM const char band_DaySnowFogRGB[] = "Day Snow-Fog RGB";
PROGMEM const char band_NaturalColorRGB[] = "Natural Color RGB";
PROGMEM const char band_TrueColorRGB[] = "True Color RGB (Enhanced)";
PROGMEM const char band_TrueColorReproductionImage[] = "True Color Reproduction Image";
PROGMEM const char band_DayConvectiveStormRGB[] = "Day Convective Storm RGB";
PROGMEM const char band_Sandwich[] = "Sandwich";
PROGMEM const char band_B03CombinedWithB13[] = "B03 combined with B13";
PROGMEM const char band_B03AndB13AtNight[] = "B03 and B13 at night";
PROGMEM const char band_HeavyRainfallPotentialAreas[] = "Heavy rainfall potential areas";

PROGMEM const char* bandTable[] =
{
    band_B13,
    band_B03,
    band_B08,
    band_B07,
    band_DayMicrophysicsRGB,
    band_NightMicrophysicsRGB,
    band_DustRGB,
    band_AirmassRGB,
    band_DaySnowFogRGB,
    band_NaturalColorRGB,
    band_TrueColorRGB,
    band_TrueColorReproductionImage,
    band_DayConvectiveStormRGB,
    band_Sandwich,
    band_B03CombinedWithB13,
    band_B03AndB13AtNight,
    band_HeavyRainfallPotentialAreas,
};
//
}

namespace himawari
{

String bandToString(const Band band)
{
    return bandTable[gob::to_underlying(band)];
}
//
}
