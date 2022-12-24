/*
  Icon indicating access to WiFi or BT in progress
 */

#include "progress_icon.hpp"
#include "res/resource.h"

namespace
{
constexpr int16_t ICON_X = 284;
constexpr int16_t ICON_Y = 160;

inline int32_t roundTrip(const int32_t c)
{
    //    return ( (c & 256) + ( (c & 255) ^ (0 - ((c >> 8) & 1)) ) );
    return ( (c & 128) + ( (c & 127) ^ (0 - ((c >> 7) & 1)) ) );
}

//
}

ProgressIcon::ProgressIcon()
{
    _btSprite = new LGFX_Sprite();
    assert(_btSprite);
    _btSprite->createFromBmp(bt_bmp, bt_bmp_len);

    _wifiSprite = new LGFX_Sprite();
    assert(_wifiSprite);
    _wifiSprite->createFromBmp(bt_bmp, bt_bmp_len);
}


// { _bt = bt; _wifi = wifi; }

void ProgressIcon::render(m5gfx::M5GFX* dst)
{
    auto v = roundTrip(_cnt);
    _btSprite->setPaletteColor  (1, v * 2, v * 2, v * 2);
    _wifiSprite->setPaletteColor(1, v * 2, v * 2, v * 2);

    if(_bt)   { _btSprite->pushSprite(dst, ICON_X, ICON_Y, 0); }
    if(_wifi) { _wifiSprite->pushSprite(dst, ICON_X, ICON_Y, 0); }
}
