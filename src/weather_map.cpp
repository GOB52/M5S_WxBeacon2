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

std::map<jma::weathercode_t, const uint8_t*> pngTable =
{
    { 100, w100_png },
    { 101, w101_png },
    { 102, w102_png },
    { 104, w104_png },
    { 110, w110_png },
    { 112, w112_png },
    { 115, w115_png },
    { 200, w200_png },
    { 201, w201_png },
    { 202, w202_png },
    { 204, w204_png },
    { 210, w210_png },
    { 212, w212_png },
    { 215, w215_png },
    { 300, w300_png },
    { 301, w301_png },
    { 302, w302_png },
    { 303, w303_png },
    { 308, w308_png },
    { 311, w311_png },
    { 313, w313_png },
    { 314, w314_png },
    { 400, w400_png },
    { 401, w401_png },
    { 402, w402_png },
    { 403, w403_png },
    { 406, w406_png },
    { 411, w411_png },
    { 413, w413_png },
    { 414, w414_png },
};

// WARNING : Keep largets free heap 50K for decode PNG.
void drawWeatherIcon(m5gfx::M5GFX* dst, jma::officecode_t oc, jma::weathercode_t wc)
{
    auto ic = jma::weatherCodeToIcon(wc);
    if(!ic || pngTable.count(ic) == 0 || positionTable.count(oc) == 0) { return; }

    auto png = pngTable.at(ic);
    auto pos = positionTable.at(oc);
    //    WB2_LOGD("%d:%d:%d p:%d", oc, wc, ic, png != nullptr);
    auto ret = dst->drawPng(png, -1, pos->x * 2, pos->y * 2);
    if(ret == 0)
    {
        WB2_LOGE("Failed to drawPn largeest internal:%u", heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
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
