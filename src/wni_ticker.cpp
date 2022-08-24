/*
  WNI style ticker
 */
#include "wni_ticker.hpp"
#include "res/resource.h"
#include "u8g2_font_profont22_tr.h"
#include <ctime>
#include <cassert>

namespace
{
const lgfx::U8g2font profont22( u8g2_font_profont22_tr );
//
};

const uint32_t Ticker::DECOR_FOREGROUND_COLORS[] =
{
    ::lgfx::LGFXBase::color888(  0,   0, 255), // blue
    ::lgfx::LGFXBase::color888(188, 188,   0), // yellow
    ::lgfx::LGFXBase::color888(255,   0,   0), // red
    ::lgfx::LGFXBase::color888(128,   0, 255), // purple
};

Ticker::Ticker()
        : _telop(nullptr)
        , _str{}
        , _twidth(0)
        , _x(0)
        , _decorLeft(nullptr)
        , _decorRight(nullptr)
        , _logo(nullptr)
        , _level(Level::Blue)
        , _show(true)
{
    //
    _telop = new LGFX_Sprite();
    _telop->setColorDepth(1); // 1bpp
    _telop->createSprite(TELOP_WIDTH, TELOP_HEIGHT);
    _telop->setBitmapColor(::lgfx::LGFXBase::color24to16(TELOP_FCLR), ::lgfx::LGFXBase::color24to16(TELOP_BCLR));
    _telop->setTextColor(1);
    _telop->setFont(&profont22);

    //
    _decorRight = new LGFX_Sprite();
    assert(_decorRight);
    _decorRight->setColorDepth(1);
    _decorRight->createSprite(DECOR_RIGHT_WIDTH, DECOR_RIGHT_HEIGHT);
    _decorRight->setBitmapColor(::lgfx::LGFXBase::color24to16(DECOR_FOREGROUND_COLORS[_level]), ::lgfx::LGFXBase::color24to16(TELOP_BCLR));
    _decorRight->setTextColor(1);
    _decorRight->setFont(&fonts::Font0);

    //
    _decorLeft = new LGFX_Sprite();
    assert(_decorLeft);
    _decorLeft->setColorDepth(2);
    _decorLeft->createSprite(DECOR_LEFT_WIDTH, DECOR_LEFT_HEIGHT);
    _decorLeft->setPaletteColor(0, 255,0,255); // 0: transparent color
    ::lgfx::rgb888_t rgb(DECOR_FOREGROUND_COLORS[_level]);
    _decorLeft->setPaletteColor(1, rgb.R8(), rgb.G8(), rgb.B8()); // 1: level color
    _decorLeft->setPaletteColor(2, 255, 255, 255); // 2: text color
    _decorLeft->setFont(&profont22);
    _decorLeft->setTextColor(2);

    _decorLeft->clear();
    _decorLeft->fillRect(0,0,DECOR_LEFT_WIDTH, DECOR_LEFT_HEIGHT / 2, 1);
    _decorLeft->fillRect(0,0,DECOR_LEFT_GAP, DECOR_LEFT_HEIGHT, 1);
    _decorLeft->drawString("Ponko", 1, 1);

    //
    _logo = new LGFX_Sprite();
    assert(_logo);
    _logo->createFromBmp(logo_bmp, logo_bmp_len);
    _logo->setPaletteColor(1, rgb.R8(), rgb.G8(), rgb.B8()); // 1: level color
}

Ticker::~Ticker()
{
    delete _logo;
    delete _decorRight;
    delete _decorLeft;
    delete _telop;
}

void Ticker::setLevel(const Level lv)
{
    _level = lv;
    ::lgfx::rgb888_t rgb(DECOR_FOREGROUND_COLORS[_level]);
    _decorLeft->setPaletteColor (1, rgb.R8(), rgb.G8(), rgb.B8());
    _decorRight->setPaletteColor(1, rgb.R8(), rgb.G8(), rgb.B8());
}

// Set level by WBGT
// see also https://www.wbgt.env.go.jp/wbgt.php
void Ticker::setLevelWBGT(float wbgt)
{
    Level lv = (wbgt < 25.0f) ? Level::Blue
            : (wbgt < 31.0f) ? Level::Yellow
            : Level::Red;
    setLevel(lv);
}

void Ticker::setText(const char* str)
{
    _x = _telop->width();
    _str[0] = '\0';
    _twidth = 0;
    
    if(str && str[0])
    {
        strncpy(_str, str, sizeof(_str) - 1);
        _str[sizeof(_str) - 1] = '\0';
        _twidth = _telop->textWidth(_str);
    }
}

void Ticker::pump()
{
    // Telop
    _telop->clear(0);
    if(_str[0])
    {
        auto bwid = _telop->width();
        _telop->drawString(_str, _x, 0);
        // Connect string if string width greater than width of _telop.
        if(_twidth > bwid && (_x + _twidth) < bwid)
        {
            _telop->drawString(_str, _x + _twidth, 0);
        }
        // Infinity scroll string to left.
        _x -= 2;
        if(_twidth <= bwid)
        {
            if(_x < -_twidth) { _x = _telop->width(); }
        }
        else
        {
            if(_x < -_twidth) { _x += _twidth; }
        }
    }

    // Localtime
    std::tm t;
    _decorRight->clear();
    _logo->pushSprite(_decorRight, 0, 0, 0);
    if(getLocalTime(&t))
    {
        char clockStr[64];
        snprintf(clockStr, sizeof(clockStr), "%02d:%02d", t.tm_hour, t.tm_min);
        clockStr[sizeof(clockStr) - 1] = '\0';
        _decorRight->drawString(clockStr,
                                _decorRight->width()/2 - _decorRight->textWidth(clockStr) / 2,
                                _decorRight->height() - (_decorRight->fontHeight() + 2) );
        _decorRight->drawString(clockStr,
                                _decorRight->width()/2 - _decorRight->textWidth(clockStr) / 2 + 1,
                                _decorRight->height() - (_decorRight->fontHeight() + 2) );

    }
}

void Ticker::render(m5gfx::M5GFX* dst)
{
    if(!isShow()) { return; }

    _telop->pushSprite(dst, DECOR_LEFT_GAP, dst->height() - _telop->height());
    _decorRight->pushSprite(dst, dst->width() - (_decorRight->width() + GAP), dst->height() - _decorRight->height());
    _decorLeft->pushSprite(dst, 0, dst->height() - _decorLeft->height(), 0);
}
