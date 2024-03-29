/*
  Ponko avatar
 */
#ifndef PONKO_AVATAR_HPP
#define PONKO_AVATAR_HPP

/*
  Ponko avatar
 */
#include <M5GFX.h>
#include"jma/jma_definition.hpp"
#include <gob_datetime.hpp>
#include <vector>

class Avatar
{
  public:
    struct IconInfo { jma::officecode_t oc; jma::weathercode_t wc; };

    Avatar();
    virtual ~Avatar();

    void pump();
    void render(m5gfx::M5GFX* dst, const bool force = false)
    {
        _wipe ? renderWipe(dst, force) : renderCloseup(dst, force);
    }

    // Change rendering type.
    void closeup() { _wipe = false; }
    void wipe(const int16_t x, const int16_t y, const float scale)
    {
        _wipe = true; _wipeX = x; _wipeY = y; _wipeScale = scale;
    }
    
    constexpr static int16_t MOUTH_X = 136;
    constexpr static int16_t MOUTH_Y = 168;    
    constexpr static int16_t MOUTH_WIDTH = 48;
    constexpr static int16_t MOUTH_HEIGHT = 32;
    
  private:
    void renderCloseup(m5gfx::M5GFX* dst, bool forceRender = false);
    void renderWipe(m5gfx::M5GFX* dst, bool forceRender = false);

    enum : uint8_t {Close, Open, Max };
    LGFX_Sprite* _offMouth{};
    LGFX_Sprite* _mouth[Max]{};
    float _mouthRatio{}, _lastMouthRatio{};
    bool _wipe{};
    float _wipeScale{};
    int16_t _wipeX{}, _wipeY{};
};
#endif
