#ifndef RESOURCE_H
#define RESOURCE_H

#include <M5GFX.h>
#include <cstdint>
#include <cstddef>

/* WARNING : face, close, and open image must have the same palettes, and palette index 0 must be a transparent color.
 */
// face 4bpp bmp3
extern const uint8_t ponko_face_bmp[];
extern const size_t  ponko_face_bmp_len;
// mouth(close) 4bpp bmp3
extern const uint8_t ponko_mouth_close_bmp[];
extern const size_t  ponko_mouth_close_bmp_len;
// mouth(open) 4bpp bmp3
extern const uint8_t ponko_mouth_open_bmp[];
extern const size_t  ponko_mouth_open_bmp_len;

// 1bpp bmp3 palette 0 is background, 1 is foreground
extern const uint8_t logo_bmp[];
extern const size_t  logo_bmp_len;
extern const uint8_t jpmap_bmp[];
extern const size_t  jpmap_bmp_len;

// 1bpp WifI/BT icon
extern const uint8_t wifi_bmp[];
extern const size_t  wifi_bmp_len;
extern const uint8_t bt_bmp[];
extern const size_t  bt_bmp_len;

// Text font
extern const lgfx::U8g2font titleFont;
extern const lgfx::U8g2font tickerFont;

// Weather icon 2bpp png
extern const uint8_t w100_png[];
extern const size_t w100_png_len;

extern const uint8_t w101_png[];
extern const size_t w101_png_len;

extern const uint8_t w102_png[];
extern const size_t w102_png_len;

extern const uint8_t w104_png[];
extern const size_t w104_png_len;

extern const uint8_t w110_png[];
extern const size_t w110_png_len;

extern const uint8_t w112_png[];
extern const size_t w112_png_len;

extern const uint8_t w115_png[];
extern const size_t w115_png_len;

extern const uint8_t w200_png[];
extern const size_t w200_png_len;

extern const uint8_t w201_png[];
extern const size_t w201_png_len;

extern const uint8_t w202_png[];
extern const size_t w202_png_len;

extern const uint8_t w204_png[];
extern const size_t w204_png_len;

extern const uint8_t w210_png[];
extern const size_t w210_png_len;

extern const uint8_t w212_png[];
extern const size_t w212_png_len;

extern const uint8_t w215_png[];
extern const size_t w215_png_len;

extern const uint8_t w300_png[];
extern const size_t w300_png_len;

extern const uint8_t w301_png[];
extern const size_t w301_png_len;

extern const uint8_t w302_png[];
extern const size_t w302_png_len;

extern const uint8_t w303_png[];
extern const size_t w303_png_len;

extern const uint8_t w308_png[];
extern const size_t w308_png_len;

extern const uint8_t w311_png[];
extern const size_t w311_png_len;

extern const uint8_t w313_png[];
extern const size_t w313_png_len;

extern const uint8_t w314_png[];
extern const size_t w314_png_len;

extern const uint8_t w400_png[];
extern const size_t w400_png_len;

extern const uint8_t w401_png[];
extern const size_t w401_png_len;

extern const uint8_t w402_png[];
extern const size_t w402_png_len;

extern const uint8_t w403_png[];
extern const size_t w403_png_len;

extern const uint8_t w406_png[];
extern const size_t w406_png_len;

extern const uint8_t w411_png[];
extern const size_t w411_png_len;

extern const uint8_t w413_png[];
extern const size_t w413_png_len;

extern const uint8_t w414_png[];
extern const size_t w414_png_len;

#endif
