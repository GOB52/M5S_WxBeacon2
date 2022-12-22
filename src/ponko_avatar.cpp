/*
  Ponko avatar
 */
#include "ponko_avatar.hpp"
#include "aq_talk.hpp"
#include "res/resource.h"
#include "wb2/wxbeacon2_log.hpp"
#include <cassert>

namespace
{
void drawFace(m5gfx::M5GFX* dst, int16_t x, int16_t y, float zoom = 1.0f)
{
    dst->drawBmp(ponko_face_bmp, -1, x, y, 0, 0, 0, 0, zoom, zoom);
}

//
}
Avatar::Avatar()
        : _offMouth(nullptr)
        , _mouth{nullptr,nullptr}
        , _mouthRatio(0)
        , _lastMouthRatio(0)
        , _wipe(false)
        , _wipeX(0)
        , _wipeY(0)
{
    // close
    auto mc = new LGFX_Sprite();
    assert(mc && "mc is nullptr");
    mc->createFromBmp(ponko_mouth_close_bmp, ponko_mouth_close_bmp_len);
    mc->setPivot(mc->width() * 0.5f + 0.5f, 0); // center,top
    _mouth[Close] = mc;    

    // open
    auto mo = new LGFX_Sprite();
    assert(mo && "mo is nullptr");
    mo->createFromBmp(ponko_mouth_open_bmp, ponko_mouth_open_bmp_len);
    mo->setPivot(mo->width() * 0.5f + 0.5f, 0); // center,top
    _mouth[Open] = mo;

    // offscreen for mouth
    _offMouth= new LGFX_Sprite();
    assert(_offMouth && "_offMouth is nullptr");
    _offMouth->createFromBmp(ponko_mouth_open_bmp, ponko_mouth_open_bmp_len); // same as open
    _offMouth->setPivot(_offMouth->width() * 0.5f + 0.5f, _offMouth->height() * 0.5f + 0.5f); // center
}

Avatar::~Avatar()
{
    delete _offMouth;
    delete _mouth[Open];
    delete _mouth[Close];
}

void Avatar::pump()
{
    if(!aq_talk::busy())
    {
        _lastMouthRatio =  _mouthRatio = 0.0f;
        return;
    }

    auto gain = aq_talk::getGain();
    float f = gain / 12000.0f;
    float ratio = std::min(1.0f, _lastMouthRatio + f / 2.0f);

    _lastMouthRatio = f;
    _mouthRatio = ratio;
}

void Avatar::renderCloseup(m5gfx::M5GFX* dst, bool force)
{
    if(force) { drawFace(dst, 0, 0); }

    _offMouth->drawBmp(ponko_face_bmp, -1, 0, 0, _offMouth->width(), _offMouth->height(), MOUTH_X, MOUTH_Y);

    if(_mouthRatio != 0.0f)
    {
        auto s = _mouth[Open];
        s->pushRotateZoom(_offMouth,
                          0 + s->width() * (s->getPivotX() / s->width()),
                          0 + s->height() * (s->getPivotY() / s->height()),
                          0.0f, 1.0f, _mouthRatio,
                          0);

    }
    else
    {
        auto s = _mouth[Close];
        s->pushSprite(_offMouth, 0, 0, 0);
    }
    _offMouth->pushSprite(dst, MOUTH_X, MOUTH_Y);
}

void Avatar::renderWipe(m5gfx::M5GFX* dst, bool force)
{
    if(force)
    {
        drawFace(dst, _wipeX, _wipeY, _wipeScale);
        dst->drawRect(_wipeX, _wipeY, 320 * _wipeScale, 240 * _wipeScale, TFT_WHITE);
    }

    _offMouth->drawBmp(ponko_face_bmp, -1, 0, 0, _offMouth->width(), _offMouth->height(), MOUTH_X, MOUTH_Y);
    if(_mouthRatio != 0.0f)
    {
        auto s = _mouth[Open];
        s->pushRotateZoom(_offMouth,
                          0 + s->width() * (s->getPivotX() / s->width()),
                          0 + s->height() * (s->getPivotY() / s->height()),
                          0.0f, 1.0f, _mouthRatio,
                          0);

    }
    else
    {
        auto s = _mouth[Close];
        s->pushSprite(_offMouth, 0, 0, 0);
    }
    _offMouth->pushRotateZoom(dst,
                              _wipeX + MOUTH_X * _wipeScale + _offMouth->getPivotX() * _wipeScale,
                              _wipeY + MOUTH_Y * _wipeScale + _offMouth->getPivotY() * _wipeScale,
                              0.0f, _wipeScale, _wipeScale, 0);
}
