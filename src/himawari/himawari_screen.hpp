/*
  Render himawari image
  WARNING : Works only with devices that have PSRAM (Need huge memories)
*/
#ifndef HIMAWARI_SCREEN_HPP
#define HIMAWARI_SCREEN_HPP

#include <M5GFX.h>
#include <gob_datetime.hpp>

class HimawariScreen
{
  public:
    HimawariScreen() = default;

    inline bool isShow() const { return _show; }
    inline void show() { show(true); }
    inline void hide() { show(false); }

    void set(const goblib::datetime::OffsetDateTime& odt, const uint8_t* jpg)
    {
        _odt = odt; _jpg = jpg;
    }
    void render(m5gfx::M5GFX* dst, const bool force);

  private:
    inline void show(bool b) { _show = b; }

  private:
    bool _show{};
    goblib::datetime::OffsetDateTime _odt{};
    const uint8_t* _jpg{};
};

#endif
