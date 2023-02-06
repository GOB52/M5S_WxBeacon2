/*
  Render himawari image
 */
#ifdef ARDUINO_M5STACK_Core2

#include "himawari_screen.hpp"
#include "wb2/wxbeacon2_log.hpp"
#include "res/resource.h"

void HimawariScreen::render(m5gfx::M5GFX* dst, const bool force)
{
    if(!isShow() || !force) { return; }
    if(_jpg)
    {
        auto ret = dst->drawJpg(_jpg, -1, 0, 0, 0, 0, 64, 24, 0.5f, 0.5f);
        WB2_LOGD("drawJpg %d", ret);
    }

    // Day of forecast
    String s = _odt.toLocalDateTime().toString("%m/%d %H:%M");
    auto ofont = dst->getFont();
    dst->setFont(&tickerFont);
    dst->setTextColor(TFT_BLACK);
    dst->setCursor(9,9);
    dst->printf("%s", s.c_str());
    dst->setCursor(8,8);
    dst->setTextColor(TFT_WHITE);
    dst->printf("%s", s.c_str());
    dst->setFont(ofont);
    dst->setTextColor(TFT_WHITE, TFT_BLACK);
}

#endif
