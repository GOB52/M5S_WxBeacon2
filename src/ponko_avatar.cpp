/*
  Ponko avatar
 */
#include "ponko_avatar.hpp"
#include "aq_talk.hpp"
#include "res/resource.h"
#include "wb2/wxbeacon2_log.hpp"
#include <map>
#include <cassert>

namespace
{
struct Pos { uint8_t x, y; }; // Half of actual position.
PROGMEM const Pos ocPos[] = {
    { 120,  8 }, // sap
    { 136, 40 }, // sen
    { 128, 72 }, // tok
    { 104, 32 }, // nii
    {  88, 48 }, // kan
    { 104, 80 }, // nag
    {  80, 72 }, // osa
    {  48, 64 }, // hir
    {  48, 84 }, // kou
    {  16, 56 }, // fuk
    {   8, 32 }  // nah
};
std::map<jma::officecode_t, const Pos*> positionTable =
{
    {  16000, &ocPos[0] },
    {  40000, &ocPos[1] },
    { 130000, &ocPos[2] },
    { 150000, &ocPos[3] },
    { 170000, &ocPos[4] },
    { 230000, &ocPos[5] },
    { 270000, &ocPos[6] },
    { 340000, &ocPos[7] },
    { 390000, &ocPos[8] },
    { 400000, &ocPos[9] },
    { 471000, &ocPos[10] },
};

#if 0
std::map<jma::weathercode_t, const uint8_t*> pngTable =
{
    { 100, w100_png },
    { 101, w101_png },
    { 102, w102_png },
    { 104, w104_png },
    { 110, w110_png },
    { 112, w112_png },
    { 115, w115_png },
    { 200, w200_png },
    { 201, w201_png },
    { 202, w202_png },
    { 204, w204_png },
    { 210, w210_png },
    { 212, w212_png },
    { 215, w215_png },
    { 300, w300_png },
    { 301, w301_png },
    { 302, w302_png },
    { 303, w303_png },
    { 311, w311_png },
    { 313, w313_png },
    { 314, w314_png },
    { 400, w400_png },
    { 401, w401_png },
    { 402, w402_png },
    { 403, w403_png },
    { 411, w411_png },
    { 413, w413_png },
    { 414, w414_png },
};
#else
struct Png { const uint8_t* p; const size_t sz; };
std::map<jma::weathercode_t, Png> pngTable =
{
    { 100, { w100_png, w100_png_len } },
    { 101, { w101_png, w101_png_len } },
    { 102, { w102_png, w102_png_len } },
    { 104, { w104_png, w104_png_len } },
    { 110, { w110_png, w110_png_len } },
    { 112, { w112_png, w112_png_len } },
    { 115, { w115_png, w115_png_len } },
    { 200, { w200_png, w200_png_len } },
    { 201, { w201_png, w201_png_len } },
    { 202, { w202_png, w202_png_len } },
    { 204, { w204_png, w204_png_len } },
    { 210, { w210_png, w210_png_len } },
    { 212, { w212_png, w212_png_len } },
    { 215, { w215_png, w215_png_len } },
    { 300, { w300_png, w300_png_len } },
    { 301, { w301_png, w301_png_len } },
    { 302, { w302_png, w302_png_len } },
    { 303, { w303_png, w303_png_len } },
    { 311, { w311_png, w311_png_len } },
    { 313, { w313_png, w313_png_len } },
    { 314, { w314_png, w314_png_len } },
    { 400, { w400_png, w400_png_len } },
    { 401, { w401_png, w401_png_len } },
    { 402, { w402_png, w402_png_len } },
    { 403, { w403_png, w403_png_len } },
    { 411, { w411_png, w411_png_len } },
    { 413, { w413_png, w413_png_len } },
    { 414, { w414_png, w414_png_len } },
};
#endif


constexpr uint32_t MAP_FCLR = ::lgfx::color888(  16, 136,  88); // palette 1
constexpr uint32_t MAP_BCLR = ::lgfx::color888(   0,  80, 160); // palette 0

// WARNING : Keep largets free heap 50K for decode PNG.
void drawWeatherIcon(m5gfx::M5GFX* dst, jma::officecode_t oc, jma::weathercode_t wc)
{
    auto ic = jma::weatherCodeToIcon(wc);
    if(!ic || pngTable.count(ic) == 0 || positionTable.count(oc) == 0) { return; }

    auto png = pngTable.at(ic);
    auto pos = positionTable.at(oc);
    WB2_LOGD("%d:%d:%d %zu", oc, wc, ic, png.sz);
    auto ret = dst->drawPng(png.p, (uint32_t)png.sz, pos->x * 2, pos->y * 2);
    if(ret == 0) { WB2_LOGV("mem:%u", esp_get_free_internal_heap_size()); }
}

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
        , _iconInfo(11)
{
    // close
    auto mc = new LGFX_Sprite();
    assert(mc && "mc is nullptr");
    mc->createFromBmp(ponko_mouth_close_bmp, ponko_mouth_close_bmp_len);
    _mouth[Close] = mc;
    mc->setPivot(mc->width() * 0.5f + 0.5f, 0); // center,top
    
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

void Avatar::renderCloseup(m5gfx::M5GFX* dst, bool forceRender)
{
    if(forceRender) { drawFace(dst, 0, 0); }

    _offMouth->drawBmp(ponko_face_bmp, -1, 0, 0, _offMouth->width(), _offMouth->height(), MOUTH_X, MOUTH_Y);
    if(_mouthRatio > 1.0f)
    {
        printf("%s:%f\n", __func__, _mouthRatio);
    }

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

void Avatar::renderWipe(m5gfx::M5GFX* dst, bool forceRender)
{
    if(forceRender)
    {
        dst->drawBmp(jpmap_bmp, -1, 0, 0, 0, 0, 0, 0, 2.0f, 2.0f); // map
        
        for(auto& e : _iconInfo)
        {
            drawWeatherIcon(dst, e.oc, e.wc);
        }

        auto ofont = dst->getFont();
        dst->setFont(&tickerFont);
        dst->setTextColor(TFT_WHITE);
        dst->setCursor(8,8);
        dst->printf("%sの天気", _odt.toLocalDate().toString("%m/%d").c_str());
        dst->setFont(ofont);
        dst->setTextColor(TFT_WHITE, TFT_BLACK);

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
