/*
  WNI style ticker
 */
#include "wni_ticker.hpp"
#include "gob_datetime.hpp"
#include "wb2/wxbeacon2_log.hpp"
#include "res/resource.h"
#include "utility.hpp"
#include <ctime>
#include <cassert>

using goblib::datetime::LocalTime;

PROGMEM const uint16_t Ticker::DECOR_FOREGROUND_COLORS[] =
{
    ::lgfx::color565(  0,   0, 255), // blue
    ::lgfx::color565(188, 188,   0), // yellow
    ::lgfx::color565(255,   0,   0), // red
    ::lgfx::color565(128,   0, 255), // purple
    ::lgfx::color565( 64, 144,  48), // green
};

PROGMEM const uint16_t Ticker::TELOP_COLORS[] =
{
    ::lgfx::color565(255, 255, 255),
    ::lgfx::color565(  0,   0,   0),
    ::lgfx::color565(255,   0,   0),
    ::lgfx::color565(  0,   0, 255),
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
    _telop->setColorDepth(2); // 2bpp
    _telop->createSprite(TELOP_WIDTH, TELOP_HEIGHT);
    auto pal = _telop->getPalette();
    for(auto& e : TELOP_COLORS) { *pal++ = ::lgfx::LGFXBase::color16to24(e); }
    _telop->setTextColor(1);
    _telop->setFont(&tickerFont);

    //
    _decorRight = new LGFX_Sprite();
    assert(_decorRight);
    _decorRight->setPsram(true);
    _decorRight->setColorDepth(1); // 1bpp
    _decorRight->createSprite(DECOR_RIGHT_WIDTH, DECOR_RIGHT_HEIGHT);
    _decorRight->setBitmapColor(DECOR_FOREGROUND_COLORS[_color], BG_COLOR);
    _decorRight->setTextColor(1);
    _decorRight->setFont(&fonts::Font0);

    //
    _decorLeft = new LGFX_Sprite();
    assert(_decorLeft);
    _decorLeft->setPsram(true);
    _decorLeft->setColorDepth(2); // 4bpp
    _decorLeft->createSprite(DECOR_LEFT_WIDTH, DECOR_LEFT_HEIGHT);
    _decorLeft->setPivot(0,0); //left,top
    _decorLeft->setPaletteColor(0, 255,0,255); // 0: transparent color
    _decorLeft->setPaletteColor(1, DECOR_FOREGROUND_COLORS[_color]);
    _decorLeft->setPaletteColor(2, DECOR_TEXT_COLOR);
    _decorLeft->setFont(&titleFont);
    _decorLeft->setTextColor(2);
    _decorLeft->clear();
    _decorLeft->fillRect(0,0,DECOR_LEFT_WIDTH, DECOR_LEFT_HEIGHT / 2, 1);
    _decorLeft->fillRect(0,0,DECOR_LEFT_GAP, DECOR_LEFT_HEIGHT, 1);

    //
    _logo = new LGFX_Sprite();
    assert(_logo);
    _logo->createFromBmp(logo_bmp, logo_bmp_len);
    _logo->setPaletteColor(1, DECOR_FOREGROUND_COLORS[_color]);
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
    _decorLeft->setPaletteColor (1, DECOR_FOREGROUND_COLORS[_color]);
    _decorRight->setPaletteColor(1, DECOR_FOREGROUND_COLORS[_color]);
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
    if(strlen(str) >= STRING_LEN) { WB2_LOGE("BufferOver %zu", strlen(str)); }

    _x = _telop->width();
    strncpy(_str, str, sizeof(_str) - 1);
    _str[sizeof(_str) - 1] = '\0';

    auto tokens = gob::split(str, '#');
    _twidth = 0;
    for(auto& e : tokens)
    {
        if(e.length() == 0) { continue; }

        _twidth += _telop->textWidth(e.c_str() + (e[0] == '@' && isdigit(e[1])) * 2); // Command of change palette?
    }
}

void Ticker::pump()
{
    // Telop
    _telop->clear(0);
    if(_str[0])
    {
        auto twid = _telop->width();
        drawTelop(_str, _x, 0);
        // Connect string if string width greater than width of _telop.
        if(_twidth > twid && (_x + _twidth) < twid)
        {
            drawTelop(_str, _x + _twidth, 0);
        }
        // Infinity scroll string.
        _x -= _speed;
        if(_twidth <= twid)
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

void Ticker::drawTelop(const char* str, const int16_t x, const int16_t y)
{
    if(!_str[0] || x >= _telop->width()) { return; }
    
    int16_t left = x;
    _telop->setTextColor(1);

    auto tokens = gob::split(str, '#');
    for(auto& e : tokens)
    {
        if(left > 320) { break; }
        if(e.length() == 0) { continue; }
        const char* s = e.c_str();
        if(*s == '@' && isdigit(*(s+1))) { _telop->setTextColor((*(s+1)) - '0'); s += 2; } // Change palette

        auto w = _telop->textWidth(s);
        if(left >= 0 || left + w >= 0)
        {
            _telop->drawString(s, left, y);
        }
        left += w;
    }

}
