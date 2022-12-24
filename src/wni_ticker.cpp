/*
  WNI style ticker
 */
#include "wni_ticker.hpp"
//#include "gob_sprite.hpp"
#include "gob_datetime.hpp"
#include "wb2/wxbeacon2_log.hpp"
#include "res/resource.h"
#include <ctime>
#include <cassert>


using goblib::datetime::LocalTime;

const uint32_t Ticker::DECOR_FOREGROUND_COLORS[] =
{
    ::lgfx::LGFXBase::color888(  0,   0, 255), // blue
    ::lgfx::LGFXBase::color888(188, 188,   0), // yellow
    ::lgfx::LGFXBase::color888(255,   0,   0), // red
    ::lgfx::LGFXBase::color888(128,   0, 255), // purple
    ::lgfx::LGFXBase::color888( 64, 144,  48), // green
};

Ticker::Ticker()
        : _telop(nullptr)
        , _str{}
        , _twidth(0)
        , _x(0)
        , _speed(2)
        , _decorLeftWidth(DECOR_LEFT_WIDTH)
        , _decorLeft(nullptr)
        , _decorRight(nullptr)
        , _logo(nullptr)
        , _color(Color::Blue)
        , _show(true)
{
    //
    _telop = new LGFX_Sprite();
    assert(_telop);
    _telop->setPsram(true);
    _telop->setColorDepth(1); // 1bpp
    _telop->createSprite(TELOP_WIDTH, TELOP_HEIGHT);
    _telop->setBitmapColor(::lgfx::LGFXBase::color24to16(TELOP_FCLR), ::lgfx::LGFXBase::color24to16(TELOP_BCLR));
    _telop->setTextColor(1);
    //    _telop->setFont(&titleFont);
    _telop->setFont(&tickerFont);

    //
    _decorRight = new LGFX_Sprite();
    assert(_decorRight);
    _decorRight->setPsram(true);
    _decorRight->setColorDepth(1); // 1bpp
    _decorRight->createSprite(DECOR_RIGHT_WIDTH, DECOR_RIGHT_HEIGHT);
    _decorRight->setBitmapColor(::lgfx::LGFXBase::color24to16(DECOR_FOREGROUND_COLORS[_color]), ::lgfx::LGFXBase::color24to16(TELOP_BCLR));
    _decorRight->setTextColor(1);
    _decorRight->setFont(&fonts::Font0);

    //
    _decorLeft = new LGFX_Sprite();
    assert(_decorLeft);
    _decorLeft->setPsram(true);
    _decorLeft->setColorDepth(2); // 4bpp
    _decorLeft->createSprite(DECOR_LEFT_WIDTH, DECOR_LEFT_HEIGHT);
    _decorLeft->setPaletteColor(0, 255,0,255); // 0: transparent color
    ::lgfx::rgb888_t rgb(DECOR_FOREGROUND_COLORS[_color]);
    _decorLeft->setPivot(0,0); //left,top
    _decorLeft->setPaletteColor(1, rgb.R8(), rgb.G8(), rgb.B8()); // 1: background color
    _decorLeft->setPaletteColor(2, 255, 255, 255); // 2: text color
    _decorLeft->setFont(&titleFont);
    _decorLeft->setTextColor(2);
    _decorLeft->clear();
    _decorLeft->fillRect(0,0,DECOR_LEFT_WIDTH, DECOR_LEFT_HEIGHT / 2, 1);
    _decorLeft->fillRect(0,0,DECOR_LEFT_GAP, DECOR_LEFT_HEIGHT, 1);

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

void Ticker::setColor(const Color c)
{
    _color = c;
    ::lgfx::rgb888_t rgb(DECOR_FOREGROUND_COLORS[_color]);
    _decorLeft->setPaletteColor (1, rgb.R8(), rgb.G8(), rgb.B8());
    _decorRight->setPaletteColor(1, rgb.R8(), rgb.G8(), rgb.B8());
}

// Set level by WBGT
// see also https://www.wbgt.env.go.jp/wbgt.php
void Ticker::setLevelWBGT(const float wbgt)
{
    Color lv = (wbgt < 25.0f) ? Color::Blue
            : (wbgt < 31.0f) ? Color::Yellow
            : Color::Red;
    setColor(lv);
}

void Ticker::setTitle(const char* title)
{
    if(!title || !title[0]) { return; }

    WB2_LOGD("%s", title);
    
    _decorLeft->clear(); // fill 0

    auto sz = _decorLeft->textWidth(title);
    _decorLeft->fillRect(0,0,sz + 4 /* gap 2 + 2 */, DECOR_LEFT_HEIGHT / 2, 1);
    _decorLeft->fillRect(0,0,DECOR_LEFT_GAP, DECOR_LEFT_HEIGHT, 1);
    _decorLeft->drawString(title, 2, 1);
}

void Ticker::setText(const char* str)
{
    if(!str || !str[0]) { return; }

    WB2_LOGD("%s", str);

    _x = _telop->width();
    _str[0] = '\0';
    _twidth = 0;
    
    if(str && str[0])
    {
        strncpy(_str, str, sizeof(_str) - 1);
        _str[sizeof(_str) - 1] = '\0';
        _twidth = _telop->textWidth(_str);
    }

#if 0
    _decorLeft->setFont(&weather_font);
    _decorLeft->setTextColor(3);
    _decorLeft->drawString("0123", 2, 1);
#endif

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
        // Infinity scroll string.
        _x -= _speed;
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
    _decorRight->clear();
    _logo->pushSprite(_decorRight, 0, 0, 0);
    LocalTime lt = LocalTime::now();
    auto clockStr = lt.toString("%H;%M");
    _decorRight->drawString(clockStr.c_str(),
                            _decorRight->width()/2 - _decorRight->textWidth(clockStr) / 2,
                            _decorRight->height() - (_decorRight->fontHeight() + 2) );
    _decorRight->drawString(clockStr.c_str(),
                            _decorRight->width()/2 - _decorRight->textWidth(clockStr) / 2 + 1,
                            _decorRight->height() - (_decorRight->fontHeight() + 2) );
}

void Ticker::render(m5gfx::M5GFX* dst)
{
    if(!isShow()) { return; }

    _telop->pushSprite(dst, DECOR_LEFT_GAP, dst->height() - _telop->height());
    _decorRight->pushSprite(dst, dst->width() - (_decorRight->width() + GAP), dst->height() - _decorRight->height());
    _decorLeft->pushSprite(dst, 0, dst->height() - _decorLeft->height(), 0);
}
