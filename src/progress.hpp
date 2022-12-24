/*
  Icon indicating access to WiFi or BT in progress.
  Progress bar for download.
 */
#ifndef PROGRESS_HPP
#define PROGRESS_HPP

#include <M5GFX.h>

class ProgressIcon
{
  public:
    ProgressIcon();

    inline void pump(const bool bt, const bool wifi) { _bt = bt; _wifi = wifi; ++_cnt; }

    inline void showProgress() { showProgress(true);  }
    inline void hideProgress() { showProgress(false); }
    inline void showProgress(const bool b) { _showProgress = b; }

    inline void initRatio() { _ratio = 0.0f; _prevRatio = -1.0f; }
    inline void setRatio(const float ratio) { _ratio = ratio; }
    
    void render(m5gfx::M5GFX* dst);

  private:
    bool _bt{}, _wifi{}, _showProgress{};
    uint16_t _cnt{};
    float _ratio{}, _prevRatio{};
    LGFX_Sprite* _btSprite{};
    LGFX_Sprite* _wifiSprite{};

};
#endif
