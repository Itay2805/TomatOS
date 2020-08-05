#ifndef TOMATOS_EARLY_CONSOLE_H
#define TOMATOS_EARLY_CONSOLE_H

#include <mem/vmm.h>

/**
 * Initialize an early console that will log
 * the kernel traces to a framebuffer
 */
void init_early_console(directptr_t framebuffer, size_t width, size_t height, size_t pitch);

/**
 * Will print to the early console
 *
 * @remark
 * Will just ignore if not initialized
 */
void early_console_putc(char c);

#endif //TOMATOS_EARLY_CONSOLE_H
