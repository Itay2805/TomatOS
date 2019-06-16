#include <drivers/portio.h>
#include <memory/vmm.h>
#include <stddef.h>
#include <common.h>
#include "text.h"

/*****************************************************************
 * Simple VGA driver, just to manipulate the cursor a little
 *****************************************************************/

#define VGA_ADDR 0x3d4
#define VGA_DATA 0x3d5

#define VGA_REG_CURSOR_START 0x0a

static void vga_write(uint8_t addr, uint8_t data) {
    outb(VGA_ADDR, addr);
    outb(VGA_DATA, data);
}

static void vga_cursor_disable() {
    vga_write(VGA_REG_CURSOR_START, (uint8_t)(1 << 5));
}

/*****************************************************************
 * Text mode terminal implementation
 *****************************************************************/

static short* vmem;
static int width;
static int height;
static int x;
static int y;

void text_init(boot_info_t* info) {
    vmem = (short *) info->framebuffer.addr;
    width = (int) info->framebuffer.width;
    height = (int) info->framebuffer.height;
    vga_cursor_disable();
    text_clear();
    if(!vmm_is_mapped(kernel_address_space, (uintptr_t) PHYSICAL_ADDRESS(vmem))) {
        vmm_map(kernel_address_space, PHYSICAL_ADDRESS(vmem), vmem, PAGE_ATTR_WRITE);
    }
    vmem = PHYSICAL_ADDRESS(vmem);
}

void text_write(const char* str) {
    while(*str) {
        char c = *str++;
        switch (c) {
            case 0x00:
                break;

            case '\n': {
                x = 0;
                y++;
            } break;

            case '\t': {
                vmem[x + y * width] = (short) (' ' | 0x0f00);
                x++;
                vmem[x + y * width] = (short) (' ' | 0x0f00);
                x++;
            }break;

            default: {
                vmem[x + y * width] = (short) (c | 0x0f00);
                x++;
            }
        }

        if(x >= width) {
            x = 0;
            y++;
        }

        if (y >= height) {
            y = height - 1;
            x = 0;
            text_scroll(1);
        }
    }
}

void text_clear() {
    for (short* i = vmem; i < vmem + width * height; i += 2) {
        *i = 0;
    }
}

void text_scroll(uint16_t n) {
    int length_copy = ((height - n) * width);
    int length_remove = (n * width);

    for (int i = 0; i < length_copy; i++) {
        vmem[i] = vmem[i + (n * width)];
    }

    for (int i = 0; i < length_remove; i++) {
        vmem[length_copy + i] = 0;
    }
}
