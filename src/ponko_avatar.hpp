/*
  Ponko avatar
 */
#ifndef PONKO_AVATAR_HPP
#define PONKO_AVATAR_HPP

#include <M5GFX.h>
#include "gob_sprite.hpp"

class Avatar
{
  public:
    Avatar();
    virtual ~Avatar();

    void pump();
    void render(m5gfx::M5GFX* dst, bool forceFace = false);

    constexpr static int16_t MOUTH_X = 136;
    constexpr static int16_t MOUTH_Y = 168;       
    constexpr static int16_t MOUTH_WIDTH = 48;
    constexpr static int16_t MOUTH_HEIGHT = 32;
    
  private:
    enum {Close, Open, Max };
    gob::Sprite* _face;
    LGFX_Sprite* _offMouth;
    LGFX_Sprite* _mouth[Max];
    float _mouthRatio, _lastMouthRatio;
};
#endif
