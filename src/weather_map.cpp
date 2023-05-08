/*
  Weather map
 */
#include "weather_map.hpp"
#include <gob_datetime.hpp>
#include "wb2/wxbeacon2_log.hpp"
#include "res/resource.h"
#include <map>

namespace
{
struct Pos { uint8_t x, y; }; // Half of actual position.
PROGMEM const Pos ocPos[] = {
    { 120,  8 }, // sap
    { 136, 40 }, // sen
    { 128, 72 }, // tok
    { 104, 32 }, // nii
    {  88, 48 }, // kan
    { 104, 80 }, // nag
    {  80, 72 }, // osa
    {  48, 64 }, // hir
    {  48, 84 }, // kou
    {  16, 56 }, // fuk
    {   8, 32 }  // nah
};

std::map<jma::officecode_t, const Pos*> positionTable =
{
    {  16000, &ocPos[0] },
    {  40000, &ocPos[1] },
    { 130000, &ocPos[2] },
    { 150000, &ocPos[3] },
    { 170000, &ocPos[4] },
    { 230000, &ocPos[5] },
    { 270000, &ocPos[6] },
    { 340000, &ocPos[7] },
    { 390000, &ocPos[8] },
    { 400000, &ocPos[9] },
    { 471000, &ocPos[10] },
};

std::map<jma::weathercode_t, const uint8_t*> iconTable =
{
    { 100, w100_jpg },
    { 101, w101_jpg },
    { 102, w102_jpg },
    { 104, w104_jpg },
    { 110, w110_jpg },
    { 112, w112_jpg },
    { 115, w115_jpg },
    { 200, w200_jpg },
    { 201, w201_jpg },
    { 202, w202_jpg },
    { 204, w204_jpg },
    { 210, w210_jpg },
    { 212, w212_jpg },
    { 215, w215_jpg },
    { 300, w300_jpg },
    { 301, w301_jpg },
    { 302, w302_jpg },
    { 303, w303_jpg },
    { 308, w308_jpg },
    { 311, w311_jpg },
    { 313, w313_jpg },
    { 314, w314_jpg },
    { 400, w400_jpg },
    { 401, w401_jpg },
    { 402, w402_jpg },
    { 403, w403_jpg },
    { 406, w406_jpg },
    { 411, w411_jpg },
    { 413, w413_jpg },
    { 414, w414_jpg },
};

// WARNING : Keep largets free heap 50K for decode PNG.
void drawWeatherIcon(m5gfx::M5GFX* dst, jma::officecode_t oc, jma::weathercode_t wc)
{
    auto ic = jma::weatherCodeToIcon(wc);
    if(!ic || iconTable.count(ic) == 0 || positionTable.count(oc) == 0) { return; }

    auto jpg = iconTable.at(ic);
    auto pos = positionTable.at(oc);
    //    WB2_LOGD("%d:%d:%d p:%d", oc, wc, ic, png != nullptr);
    auto ret = dst->drawJpg(jpg, -1, pos->x * 2, pos->y * 2);
    if(ret == 0)
    {
        WB2_LOGE("Failed to drawJpg largeest internal:%u", heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
    }
}
//
}

void WeatherMap::render(m5gfx::M5GFX* dst, bool force)
{
    if(!isShow() || !force) { return; }

    dst->drawBmp(jpmap_bmp, -1, 0, 0, 0, 0, 0, 0, 2.0f, 2.0f); // Japan map
    // Icon
    for(auto& e : _icons)
    {
        drawWeatherIcon(dst, e.oc, e.wc);
    }
    // Day of forecast
    auto ofont = dst->getFont();
    dst->setFont(&tickerFont);
    dst->setTextColor(TFT_WHITE);
    dst->setCursor(8,8);
    dst->printf("%sの天気", _odt.toLocalDate().toString("%m/%d").c_str());
    dst->setFont(ofont);
    dst->setTextColor(TFT_WHITE, TFT_BLACK);
}
