/*
 * gfxfont.h
 *
 *  Created on: 20 Dec 2018
 *      Author: Adafruit
 *      original source: https://github.com/adafruit/Adafruit-GFX-Library
 */

// Font structures for newer Adafruit_GFX (1.1 and later).
// Example fonts are included in 'Fonts' directory.
// To use a font in your Arduino sketch, #include the corresponding .h
// file and pass address of GFXfont struct to setFont().  Pass NULL to
// revert to 'classic' fixed-space bitmap font.
#include <stdint.h>

#ifndef __GFXFONT_H__
#define __GFXFONT_H__

	/**
	 *  Font data stored PER GLYPH
	 */
	typedef struct GFXglyph {
		uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
		uint8_t  width;        ///< Bitmap dimensions in pixels
		uint8_t  height;       ///< Bitmap dimensions in pixels
		uint8_t  xAdvance;     ///< Distance to advance cursor (x axis)
		int8_t   xOffset;      ///< X dist from cursor pos to UL corner
		int8_t   yOffset;      ///< Y dist from cursor pos to UL corner
	} GFXglyph;

	/// Data stored for FONT AS A WHOLE
	typedef struct GFXfont {
		uint8_t  *bitmap;      ///< Glyph bitmaps, concatenated
		GFXglyph *glyph;       ///< Glyph array
		uint8_t   first;       ///< ASCII extents (first char)
		uint8_t   last;        ///< ASCII extents (last char)
		uint8_t   yAdvance;    ///< Newline distance (y axis)
	} GFXfont;


#endif /* __GFXFONT_H__ */
