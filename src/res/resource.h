#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <cstdint>
#include <cstddef>

/* WARNING : face, close, and open image must have the same palettes, and palette index 0 must be a transparent color.
 */
// face 4bpp
extern const uint8_t ponko_face_bmp[];
extern const size_t  ponko_face_bmp_len;
// mouth(close) 4bpp
extern const uint8_t ponko_mouth_close_bmp[];
extern const size_t  ponko_mouth_close_bmp_len;
// mouth(open)4bpp
extern const uint8_t ponko_mouth_open_bmp[];
extern const size_t  ponko_mouth_open_bmp_len;

// WARNING : 1bpp palette 0 is background, 1 is foreground
extern const uint8_t logo_bmp[];
extern const size_t  logo_bmp_len;

#endif
