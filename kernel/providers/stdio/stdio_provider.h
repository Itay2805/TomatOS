//
// Created by Itay on 08/05/2019.
//

#ifndef TOMATKERNEL_TERM_H
#define TOMATKERNEL_TERM_H

#include <common/except.h>

#define KEYS_ESC            0x1B
#define KEYS_BACKSPACE      0x08
#define KEYS_LEFT_CTRL      0xA2
#define KEYS_LEFT_SHIFT     0xA0
#define KEYS_RIGHT_SHIFT    0xA1
#define KEYS_LEFT_ALT       0x12
#define KEYS_CAPS_LOCK      0x14
#define KEYS_F1             0x70
#define KEYS_F2             0x71
#define KEYS_F3             0x72
#define KEYS_F4             0x73
#define KEYS_F5             0x74
#define KEYS_F6             0x75
#define KEYS_F7             0x76
#define KEYS_F8             0x77
#define KEYS_F9             0x78
#define KEYS_F10            0x79
#define KEYS_NUM_LOCK       0x90
#define KEYS_SCROLL_LOCK    0x91
#define KEYS_F11            0x7A
#define KEYS_F12            0x7B

#define STDIN_MODE          1
#define STDIN_MODE_RAW      0
#define STDIN_MODE_CHAR     1

// TODO: Add more keycodes

error_t stdio_provider_init();

#endif //TOMATKERNEL_TERM_H
