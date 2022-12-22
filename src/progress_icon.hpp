/*
  Icon indicating access to WiFi or BT in progress
 */
#ifndef PROGRESS_ICON_HPP
#define PROGRESS_ICON_HPP

#include <M5GFX.h>

class ProgressIcon
{
  public:
    ProgressIcon();

    inline void pump(const bool bt, const bool wifi) { _bt = bt; _wifi = wifi; ++_cnt; }
    void render(m5gfx::M5GFX* dst);

  private:
    bool _bt{}, _wifi{};
    uint16_t _cnt{};
    LGFX_Sprite* _btSprite{};
    LGFX_Sprite* _wifiSprite{};

};
#endif
