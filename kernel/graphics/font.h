#ifndef GRAPHICS_FONT_H
#define GRAPHICS_FONT_H

#include "../common/stdint.h"

////////////////////////////////////////////////////////////////////////////
// Font
////////////////////////////////////////////////////////////////////////////
//
// This is the default font for the terminal.
//
// Currently only supports basic latin characters (first 128 
// characters of ASCII)
//
////////////////////////////////////////////////////////////////////////////

/**
 * This is a bitmap font for the os
 * 
 * This basically only has latin characters
 */
extern char font_basic[128][8];

#endif