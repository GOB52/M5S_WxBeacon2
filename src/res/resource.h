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

// Weather icon jpg
extern const unsigned char w100_jpg[];
extern const unsigned int w100_jpg_len;

extern const unsigned char w101_jpg[];
extern const unsigned int w101_jpg_len;

extern const unsigned char w102_jpg[];
extern const unsigned int w102_jpg_len;

extern const unsigned char w104_jpg[];
extern const unsigned int w104_jpg_len;

extern const unsigned char w110_jpg[];
extern const unsigned int w110_jpg_len;

extern const unsigned char w112_jpg[];
extern const unsigned int w112_jpg_len;

extern const unsigned char w115_jpg[];
extern const unsigned int w115_jpg_len;

extern const unsigned char w200_jpg[];
extern const unsigned int w200_jpg_len;

extern const unsigned char w201_jpg[];
extern const unsigned int w201_jpg_len;

extern const unsigned char w202_jpg[];
extern const unsigned int w202_jpg_len;

extern const unsigned char w204_jpg[];
extern const unsigned int w204_jpg_len;

extern const unsigned char w210_jpg[];
extern const unsigned int w210_jpg_len;

extern const unsigned char w212_jpg[];
extern const unsigned int w212_jpg_len;

extern const unsigned char w215_jpg[];
extern const unsigned int w215_jpg_len;

extern const unsigned char w300_jpg[];
extern const unsigned int w300_jpg_len;

extern const unsigned char w301_jpg[];
extern const unsigned int w301_jpg_len;

extern const unsigned char w302_jpg[];
extern const unsigned int w302_jpg_len;

extern const unsigned char w303_jpg[];
extern const unsigned int w303_jpg_len;

extern const unsigned char w308_jpg[];
extern const unsigned int w308_jpg_len;

extern const unsigned char w311_jpg[];
extern const unsigned int w311_jpg_len;

extern const unsigned char w313_jpg[];
extern const unsigned int w313_jpg_len;

extern const unsigned char w314_jpg[];
extern const unsigned int w314_jpg_len;

extern const unsigned char w400_jpg[];
extern const unsigned int w400_jpg_len;

extern const unsigned char w401_jpg[];
extern const unsigned int w401_jpg_len;

extern const unsigned char w402_jpg[];
extern const unsigned int w402_jpg_len;

extern const unsigned char w403_jpg[];
extern const unsigned int w403_jpg_len;

extern const unsigned char w406_jpg[];
extern const unsigned int w406_jpg_len;

extern const unsigned char w411_jpg[];
extern const unsigned int w411_jpg_len;

extern const unsigned char w413_jpg[];
extern const unsigned int w413_jpg_len;

extern const unsigned char w414_jpg[];
extern const unsigned int w414_jpg_len;

#endif
