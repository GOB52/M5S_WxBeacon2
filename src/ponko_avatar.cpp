/*
  Ponko avatar
 */
#include "ponko_avatar.hpp"
#include "aq_talk.hpp"
#include "res/resource.h"
#include <cassert>

Avatar::Avatar()
        : _face(nullptr)
        , _offMouth(nullptr)
        , _mouth{nullptr,nullptr}
        , _mouthRatio(0)
        , _lastMouthRatio(0)
{
    _face = new gob::Sprite();
    assert(_face && "_face is nullptr");
    _face->createFromBmp(ponko_face_bmp, ponko_face_bmp_len);

    auto mc = new LGFX_Sprite();
    assert(mc && "mc is nullptr");
    mc->createFromBmp(ponko_mouth_close_bmp, ponko_mouth_close_bmp_len);
    _mouth[Close] = mc;
    
    auto mo = new LGFX_Sprite();
    assert(mo && "mo is nullptr");
    mo->createFromBmp(ponko_mouth_open_bmp, ponko_mouth_open_bmp_len);
    mo->setPivot(mo->width()/2, 0);
    
    _mouth[Open] = mo;

    // offscreen for mouth
    _offMouth= new LGFX_Sprite();
    assert(_offMouth && "_offMouth is nullptr");
    _offMouth->createFromBmp(ponko_mouth_open_bmp, ponko_mouth_open_bmp_len); // same as mouth open/close bitmap palette and width/height

    // check
    assert(mc->width() == mo->width() && "different widths");
    assert(mc->height() == mo->height() && "different heights");
    assert(_offMouth->width() == mo->width() && "different widths");
    assert(_offMouth->height() == mo->height() && "different heights");

    assert(_face->getPaletteCount() == 16 && "not 4bpp");
    assert(mc->getPaletteCount() == 16 && "not 4bpp");
    assert(mo->getPaletteCount() == 16 && "not 4bpp");
    assert(_offMouth->getPaletteCount() == 16 && "not 4bpp");
    
    assert(memcmp(_face->getPalette(), mc->getPalette(), sizeof(RGBColor) * 16) == 0 && "diffrent palettes");
    assert(memcmp(mc->getPalette(), mo->getPalette(), sizeof(RGBColor) * 16) == 0 && "diffrent palettes");
    assert(memcmp(mo->getPalette(), _offMouth->getPalette(), sizeof(RGBColor) * 16) == 0 && "diffrent palettes");
}

Avatar::~Avatar()
{
    delete _offMouth;
    delete _mouth[Open];
    delete _mouth[Close];
    delete _face;
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

void Avatar::render(m5gfx::M5GFX* dst, bool forceFace)
{
    if(forceFace) { _face->pushSprite(dst, 0, 0); }
    
    _offMouth->clear();
    _face->pushPartial(_offMouth, 0, 0, MOUTH_WIDTH, MOUTH_HEIGHT, MOUTH_X, MOUTH_Y);
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
