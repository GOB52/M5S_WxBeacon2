/*
  LGFX_Sprite extension
  Support partial transfer (without affine transformation)
 */
#ifndef GOB_SPRITE_HPP
#define GOB_SPRITE_HPP

#include <M5GFX.h>

namespace gob
{

class Sprite : public lgfx::LGFX_Sprite
{
  public:
    Sprite(LovyanGFX* parent) : LGFX_Sprite(parent) {}
    Sprite() : LGFX_Sprite() {}

    /*!
      @brief partial transfer from LGFX_Sprite to LovyanGFX
      @param dst Target
      @param dx Destination x-coordinate
      @param dy Destination y-coordinate
      @param width Width to transfer
      @param height Height to transfer
      @param sx Source x-coordinate
      @param sy Source y-coordinate
      @param transp Transparent color or palette index
     */
    template<typename T>
    void pushPartial(LovyanGFX* dst,
                     const int32_t dx, const int32_t dy,
                     const int32_t width, const int32_t height,
                     const int32_t sx, const int32_t sy,
                     const T& transp)
    {
        push_partial(dst, dx, dy, width, height, sx, sy, _write_conv.convert(transp) & _write_conv.colormask);
    }

    /*!
      @brief partial transfer from LGFX_Sprite to LovyanGFX without transparention
      @param dst Target
      @param dx Destination x-coordinate
      @param dy Destination y-coordinate
      @param width Width to transfer
      @param height Height to transfer
      @param sx Source x-coordinate
      @param sy Source y-coordinate
     */
    void pushPartial(LovyanGFX* dst,
                     const int32_t dx, const int32_t dy,
                     const int32_t width, const int32_t height,
                     const int32_t sx, const int32_t sy)
    {
        push_partial(dst, dx, dy, width, height, sx, sy);
    }
    
  protected:
    /*
      @note Partial transfer is implemented by clipping destination and then shifting coordinate and pushing.
     */
    void push_partial(LovyanGFX* dst,
                      const int32_t dx, const int32_t dy,
                      const int32_t width, const int32_t height,
                      const int32_t sx, const int32_t sy,
                      const uint32_t transp = ::lgfx::pixelcopy_t::NON_TRANSP)
    {
        std::int32_t cx,cy,cw,ch;
        dst->getClipRect(&cx,&cy,&cw,&ch);
        dst->setClipRect(dx, dy, width, height);
    
        ::lgfx::pixelcopy_t p(_img, dst->getColorDepth(), getColorDepth(), dst->hasPalette(), _palette, transp);
        dst->pushImage(dx - sx, dy - sy, this->width(), this->height(), &p, _panel_sprite.getSpriteBuffer()->use_dma());

        dst->setClipRect(cx, cy, cw, ch);
    }
};
//
}
#endif
