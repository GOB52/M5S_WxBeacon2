/*
  WNI style ticker
 */
#ifndef WNI_TICKER_HPP
#define WNI_TICKER_HPP

#include <M5GFX.h>

//namespace gob { class Sprite; }

class Ticker
{
  public:
    enum Color : uint8_t { Blue, Yellow, Red, Purple, Green, Max };

    Ticker();
    virtual ~Ticker();

    void setTitle(const char* title);
    void setText(const char* str);

    bool isShow() const { return _show; }
    void show(const bool b) { _show = b; }
    void show() { show(true); }
    void hide() { show(false); }

    Color color() const { return _color; }
    void setColor(const Color lv);
    void setLevelWBGT(const float wbgt);

    void setScrollSpeed(const uint8_t s) { _speed = s; }
    
    void pump();
    void render(m5gfx::M5GFX* dst);

    constexpr static size_t STRING_LEN = 384;
    constexpr static int16_t GAP = 2;
    
    //Decoration left
    constexpr static int16_t DECOR_LEFT_WIDTH = 160;
    constexpr static int16_t DECOR_LEFT_HEIGHT = 48;
    constexpr static int16_t DECOR_LEFT_GAP = 4;
    //Decoration right
    constexpr static int16_t DECOR_RIGHT_WIDTH = 32;
    constexpr static int16_t DECOR_RIGHT_HEIGHT = 48;
    // Telop
    constexpr static int16_t TELOP_WIDTH = 320 - (DECOR_RIGHT_WIDTH + 2 * GAP + DECOR_LEFT_GAP);
    constexpr static int16_t TELOP_HEIGHT = 24; // height = fontHeight + (1 + 1)
    constexpr static uint32_t TELOP_FCLR = ::lgfx::color888(  0,   0,   0); // palette 1
    constexpr static uint32_t TELOP_BCLR = ::lgfx::color888(255, 255, 255); // palette 0

    // Decoration foreground colors for each level.
    static const uint32_t DECOR_FOREGROUND_COLORS[];

  private:
    // Telop
    LGFX_Sprite* _telop;
    //    gob::Sprite* _telop2;
    char _str[STRING_LEN + 1/* \0 */];
    int16_t _twidth;
    int16_t _x, _speed;
    // Decoration
    int16_t _decorLeftWidth;
    LGFX_Sprite* _decorLeft;
    LGFX_Sprite* _decorRight;
    LGFX_Sprite* _logo;

    Color _color;
    bool _show;
};

#endif
