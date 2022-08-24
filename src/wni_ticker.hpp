/*
  WNI style ticker
 */
#ifndef WNI_TICKER_HPP
#define WNI_TICKER_HPP

#include <M5GFX.h>

class Ticker
{
  public:
    enum Level : uint8_t { Blue, Yellow, Red, Purple, Max };

    Ticker();
    virtual ~Ticker();

    void setText(const char* str);

    bool isShow() const { return _show; }
    void show(bool b) { _show = b; }
    void show() { show(true); }
    void hide() { show(false); }

    Level level() const { return _level; }
    void setLevel(const Level lv);
    void setLevelWBGT(float wbgt);
    
    virtual void pump();
    virtual void render(m5gfx::M5GFX* dst);

    constexpr static size_t STRING_LEN = 256;
    constexpr static int16_t GAP = 2;
    
    //Decoration left
    constexpr static int16_t DECOR_LEFT_WIDTH = 64;
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
    char _str[STRING_LEN + 1/* \0 */];
    int16_t _twidth;
    int16_t _x;
    // Decoration
    LGFX_Sprite* _decorLeft;
    LGFX_Sprite* _decorRight;
    LGFX_Sprite* _logo;
    // Status
    Level _level;
    bool _show;
};

#endif
