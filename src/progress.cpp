/*
  Icon indicating access to WiFi or BT in progress.
  Progress bar for download.
 */

#include "progress.hpp"
#include "res/resource.h"

namespace
{
#if 0
constexpr int16_t ICON_X = 284;
constexpr int16_t ICON_Y = 160;
#else
constexpr int16_t ICON_X = 4;
constexpr int16_t ICON_Y = 160;
#endif

inline int32_t roundTrip(const int32_t c)
{
    //    return ( (c & 256) + ( (c & 255) ^ (0 - ((c >> 8) & 1)) ) );
    //return ( (c & 128) + ( (c & 127) ^ (0 - ((c >> 7) & 1)) ) );
    return ( (c & 32) + ( (c & 31) ^ (0 - ((c >> 5) & 1)) ) );
}

#if 0
constexpr int16_t  PROGRESS_X = 4;
#else
constexpr int16_t  PROGRESS_X = 40;
#endif
constexpr int16_t  PROGRESS_Y = 180;
constexpr int16_t  PROGRESS_WIDTH = 100;
constexpr int16_t  PROGRESS_HEIGHT = 8;
constexpr uint16_t PROGRESS_BG_CLR = TFT_BLACK;
constexpr uint16_t PROGRESS_FG_CLR = TFT_SKYBLUE;
//
}

ProgressIcon::ProgressIcon()
{
    _btSprite = new LGFX_Sprite();
    assert(_btSprite);
    _btSprite->createFromBmp(bt_bmp, bt_bmp_len);

    _wifiSprite = new LGFX_Sprite();
    assert(_wifiSprite);
    _wifiSprite->createFromBmp(wifi_bmp, wifi_bmp_len);
}

void ProgressIcon::render(m5gfx::M5GFX* dst)
{
    auto v = roundTrip(_cnt);
    _btSprite->setPaletteColor  (1, v * 8, v * 8, v * 8);
    _wifiSprite->setPaletteColor(1, v * 8, v * 8, v * 8);

    if(_bt)   { _btSprite->pushSprite(dst, ICON_X, ICON_Y, 0); }
    if(_wifi) { _wifiSprite->pushSprite(dst, ICON_X, ICON_Y, 0); }

    if(_showProgress && _ratio != _prevRatio)
    {
        _prevRatio = _ratio;
        int16_t wid = _ratio >= 100.0f ? PROGRESS_WIDTH - 2 : (PROGRESS_WIDTH - 2)* _ratio;
        dst->drawRect(PROGRESS_X, PROGRESS_Y, PROGRESS_WIDTH, PROGRESS_HEIGHT, TFT_WHITE);
        dst->fillRect(PROGRESS_X + 1,       PROGRESS_Y + 1,
                      wid,                      PROGRESS_HEIGHT - 2, PROGRESS_FG_CLR);
        dst->fillRect(PROGRESS_X + 1 + wid, PROGRESS_Y + 1,
                      PROGRESS_WIDTH - 2 - wid, PROGRESS_HEIGHT - 2, PROGRESS_BG_CLR);
    }
}
