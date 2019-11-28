#ifndef TOMATKERNEL_TERMINAL_H
#define TOMATKERNEL_TERMINAL_H

#include <tboot.h>

#define TERM_ROWS
#define TERM_COLS

/**
 * Initialize the terminal
 */
void terminal_init(tboot_info_t* info);

/**
 * Write a sinigle character
 *
 * @param c [IN] The character to print
 */
void terminal_write_char(char c);

#endif //TOMATKERNEL_TERMINAL_H
