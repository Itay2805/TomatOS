
#include <stdint.h>
#include <proc/event.h>
#include <util/defs.h>
#include "intrin.h"

event_t g_interrupt_events[256] = {0};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupts without signals, this applies to:
//  - exceptions
//  - scheduler tick
// TODO: this
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO:

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the handlers for general purpose interrupts
// these simply signal the event and quit
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Context for normal interrupts
 */
typedef struct interrupt_frame {
    uint64_t rip;
    uint64_t cs;
    IA32_RFLAGS rflags;
    uint64_t rsp;
    uint64_t ss;
} PACKED interrupt_frame_t;

/**
 * Define a generic interrupt handler that signalls an event
 */
#define EVENT_INTERRUPT_HANDLER(num) \
    __attribute__((interrupt)) \
    static void interrupt_handle_##num(interrupt_frame_t* frame) { \
        signal_event(g_interrupt_events[num]); \
    }

/**
 * Define all interrupt event handlers
 */
EVENT_INTERRUPT_HANDLER(0x21);
EVENT_INTERRUPT_HANDLER(0x22);
EVENT_INTERRUPT_HANDLER(0x23);
EVENT_INTERRUPT_HANDLER(0x24);
EVENT_INTERRUPT_HANDLER(0x25);
EVENT_INTERRUPT_HANDLER(0x26);
EVENT_INTERRUPT_HANDLER(0x27);
EVENT_INTERRUPT_HANDLER(0x28);
EVENT_INTERRUPT_HANDLER(0x29);
EVENT_INTERRUPT_HANDLER(0x2a);
EVENT_INTERRUPT_HANDLER(0x2b);
EVENT_INTERRUPT_HANDLER(0x2c);
EVENT_INTERRUPT_HANDLER(0x2d);
EVENT_INTERRUPT_HANDLER(0x2e);
EVENT_INTERRUPT_HANDLER(0x2f);
EVENT_INTERRUPT_HANDLER(0x30);
EVENT_INTERRUPT_HANDLER(0x31);
EVENT_INTERRUPT_HANDLER(0x32);
EVENT_INTERRUPT_HANDLER(0x33);
EVENT_INTERRUPT_HANDLER(0x34);
EVENT_INTERRUPT_HANDLER(0x35);
EVENT_INTERRUPT_HANDLER(0x36);
EVENT_INTERRUPT_HANDLER(0x37);
EVENT_INTERRUPT_HANDLER(0x38);
EVENT_INTERRUPT_HANDLER(0x39);
EVENT_INTERRUPT_HANDLER(0x3a);
EVENT_INTERRUPT_HANDLER(0x3b);
EVENT_INTERRUPT_HANDLER(0x3c);
EVENT_INTERRUPT_HANDLER(0x3d);
EVENT_INTERRUPT_HANDLER(0x3e);
EVENT_INTERRUPT_HANDLER(0x3f);
EVENT_INTERRUPT_HANDLER(0x40);
EVENT_INTERRUPT_HANDLER(0x41);
EVENT_INTERRUPT_HANDLER(0x42);
EVENT_INTERRUPT_HANDLER(0x43);
EVENT_INTERRUPT_HANDLER(0x44);
EVENT_INTERRUPT_HANDLER(0x45);
EVENT_INTERRUPT_HANDLER(0x46);
EVENT_INTERRUPT_HANDLER(0x47);
EVENT_INTERRUPT_HANDLER(0x48);
EVENT_INTERRUPT_HANDLER(0x49);
EVENT_INTERRUPT_HANDLER(0x4a);
EVENT_INTERRUPT_HANDLER(0x4b);
EVENT_INTERRUPT_HANDLER(0x4c);
EVENT_INTERRUPT_HANDLER(0x4d);
EVENT_INTERRUPT_HANDLER(0x4e);
EVENT_INTERRUPT_HANDLER(0x4f);
EVENT_INTERRUPT_HANDLER(0x50);
EVENT_INTERRUPT_HANDLER(0x51);
EVENT_INTERRUPT_HANDLER(0x52);
EVENT_INTERRUPT_HANDLER(0x53);
EVENT_INTERRUPT_HANDLER(0x54);
EVENT_INTERRUPT_HANDLER(0x55);
EVENT_INTERRUPT_HANDLER(0x56);
EVENT_INTERRUPT_HANDLER(0x57);
EVENT_INTERRUPT_HANDLER(0x58);
EVENT_INTERRUPT_HANDLER(0x59);
EVENT_INTERRUPT_HANDLER(0x5a);
EVENT_INTERRUPT_HANDLER(0x5b);
EVENT_INTERRUPT_HANDLER(0x5c);
EVENT_INTERRUPT_HANDLER(0x5d);
EVENT_INTERRUPT_HANDLER(0x5e);
EVENT_INTERRUPT_HANDLER(0x5f);
EVENT_INTERRUPT_HANDLER(0x60);
EVENT_INTERRUPT_HANDLER(0x61);
EVENT_INTERRUPT_HANDLER(0x62);
EVENT_INTERRUPT_HANDLER(0x63);
EVENT_INTERRUPT_HANDLER(0x64);
EVENT_INTERRUPT_HANDLER(0x65);
EVENT_INTERRUPT_HANDLER(0x66);
EVENT_INTERRUPT_HANDLER(0x67);
EVENT_INTERRUPT_HANDLER(0x68);
EVENT_INTERRUPT_HANDLER(0x69);
EVENT_INTERRUPT_HANDLER(0x6a);
EVENT_INTERRUPT_HANDLER(0x6b);
EVENT_INTERRUPT_HANDLER(0x6c);
EVENT_INTERRUPT_HANDLER(0x6d);
EVENT_INTERRUPT_HANDLER(0x6e);
EVENT_INTERRUPT_HANDLER(0x6f);
EVENT_INTERRUPT_HANDLER(0x70);
EVENT_INTERRUPT_HANDLER(0x71);
EVENT_INTERRUPT_HANDLER(0x72);
EVENT_INTERRUPT_HANDLER(0x73);
EVENT_INTERRUPT_HANDLER(0x74);
EVENT_INTERRUPT_HANDLER(0x75);
EVENT_INTERRUPT_HANDLER(0x76);
EVENT_INTERRUPT_HANDLER(0x77);
EVENT_INTERRUPT_HANDLER(0x78);
EVENT_INTERRUPT_HANDLER(0x79);
EVENT_INTERRUPT_HANDLER(0x7a);
EVENT_INTERRUPT_HANDLER(0x7b);
EVENT_INTERRUPT_HANDLER(0x7c);
EVENT_INTERRUPT_HANDLER(0x7d);
EVENT_INTERRUPT_HANDLER(0x7e);
EVENT_INTERRUPT_HANDLER(0x7f);
EVENT_INTERRUPT_HANDLER(0x80);
EVENT_INTERRUPT_HANDLER(0x81);
EVENT_INTERRUPT_HANDLER(0x82);
EVENT_INTERRUPT_HANDLER(0x83);
EVENT_INTERRUPT_HANDLER(0x84);
EVENT_INTERRUPT_HANDLER(0x85);
EVENT_INTERRUPT_HANDLER(0x86);
EVENT_INTERRUPT_HANDLER(0x87);
EVENT_INTERRUPT_HANDLER(0x88);
EVENT_INTERRUPT_HANDLER(0x89);
EVENT_INTERRUPT_HANDLER(0x8a);
EVENT_INTERRUPT_HANDLER(0x8b);
EVENT_INTERRUPT_HANDLER(0x8c);
EVENT_INTERRUPT_HANDLER(0x8d);
EVENT_INTERRUPT_HANDLER(0x8e);
EVENT_INTERRUPT_HANDLER(0x8f);
EVENT_INTERRUPT_HANDLER(0x90);
EVENT_INTERRUPT_HANDLER(0x91);
EVENT_INTERRUPT_HANDLER(0x92);
EVENT_INTERRUPT_HANDLER(0x93);
EVENT_INTERRUPT_HANDLER(0x94);
EVENT_INTERRUPT_HANDLER(0x95);
EVENT_INTERRUPT_HANDLER(0x96);
EVENT_INTERRUPT_HANDLER(0x97);
EVENT_INTERRUPT_HANDLER(0x98);
EVENT_INTERRUPT_HANDLER(0x99);
EVENT_INTERRUPT_HANDLER(0x9a);
EVENT_INTERRUPT_HANDLER(0x9b);
EVENT_INTERRUPT_HANDLER(0x9c);
EVENT_INTERRUPT_HANDLER(0x9d);
EVENT_INTERRUPT_HANDLER(0x9e);
EVENT_INTERRUPT_HANDLER(0x9f);
EVENT_INTERRUPT_HANDLER(0xa0);
EVENT_INTERRUPT_HANDLER(0xa1);
EVENT_INTERRUPT_HANDLER(0xa2);
EVENT_INTERRUPT_HANDLER(0xa3);
EVENT_INTERRUPT_HANDLER(0xa4);
EVENT_INTERRUPT_HANDLER(0xa5);
EVENT_INTERRUPT_HANDLER(0xa6);
EVENT_INTERRUPT_HANDLER(0xa7);
EVENT_INTERRUPT_HANDLER(0xa8);
EVENT_INTERRUPT_HANDLER(0xa9);
EVENT_INTERRUPT_HANDLER(0xaa);
EVENT_INTERRUPT_HANDLER(0xab);
EVENT_INTERRUPT_HANDLER(0xac);
EVENT_INTERRUPT_HANDLER(0xad);
EVENT_INTERRUPT_HANDLER(0xae);
EVENT_INTERRUPT_HANDLER(0xaf);
EVENT_INTERRUPT_HANDLER(0xb0);
EVENT_INTERRUPT_HANDLER(0xb1);
EVENT_INTERRUPT_HANDLER(0xb2);
EVENT_INTERRUPT_HANDLER(0xb3);
EVENT_INTERRUPT_HANDLER(0xb4);
EVENT_INTERRUPT_HANDLER(0xb5);
EVENT_INTERRUPT_HANDLER(0xb6);
EVENT_INTERRUPT_HANDLER(0xb7);
EVENT_INTERRUPT_HANDLER(0xb8);
EVENT_INTERRUPT_HANDLER(0xb9);
EVENT_INTERRUPT_HANDLER(0xba);
EVENT_INTERRUPT_HANDLER(0xbb);
EVENT_INTERRUPT_HANDLER(0xbc);
EVENT_INTERRUPT_HANDLER(0xbd);
EVENT_INTERRUPT_HANDLER(0xbe);
EVENT_INTERRUPT_HANDLER(0xbf);
EVENT_INTERRUPT_HANDLER(0xc0);
EVENT_INTERRUPT_HANDLER(0xc1);
EVENT_INTERRUPT_HANDLER(0xc2);
EVENT_INTERRUPT_HANDLER(0xc3);
EVENT_INTERRUPT_HANDLER(0xc4);
EVENT_INTERRUPT_HANDLER(0xc5);
EVENT_INTERRUPT_HANDLER(0xc6);
EVENT_INTERRUPT_HANDLER(0xc7);
EVENT_INTERRUPT_HANDLER(0xc8);
EVENT_INTERRUPT_HANDLER(0xc9);
EVENT_INTERRUPT_HANDLER(0xca);
EVENT_INTERRUPT_HANDLER(0xcb);
EVENT_INTERRUPT_HANDLER(0xcc);
EVENT_INTERRUPT_HANDLER(0xcd);
EVENT_INTERRUPT_HANDLER(0xce);
EVENT_INTERRUPT_HANDLER(0xcf);
EVENT_INTERRUPT_HANDLER(0xd0);
EVENT_INTERRUPT_HANDLER(0xd1);
EVENT_INTERRUPT_HANDLER(0xd2);
EVENT_INTERRUPT_HANDLER(0xd3);
EVENT_INTERRUPT_HANDLER(0xd4);
EVENT_INTERRUPT_HANDLER(0xd5);
EVENT_INTERRUPT_HANDLER(0xd6);
EVENT_INTERRUPT_HANDLER(0xd7);
EVENT_INTERRUPT_HANDLER(0xd8);
EVENT_INTERRUPT_HANDLER(0xd9);
EVENT_INTERRUPT_HANDLER(0xda);
EVENT_INTERRUPT_HANDLER(0xdb);
EVENT_INTERRUPT_HANDLER(0xdc);
EVENT_INTERRUPT_HANDLER(0xdd);
EVENT_INTERRUPT_HANDLER(0xde);
EVENT_INTERRUPT_HANDLER(0xdf);
EVENT_INTERRUPT_HANDLER(0xe0);
EVENT_INTERRUPT_HANDLER(0xe1);
EVENT_INTERRUPT_HANDLER(0xe2);
EVENT_INTERRUPT_HANDLER(0xe3);
EVENT_INTERRUPT_HANDLER(0xe4);
EVENT_INTERRUPT_HANDLER(0xe5);
EVENT_INTERRUPT_HANDLER(0xe6);
EVENT_INTERRUPT_HANDLER(0xe7);
EVENT_INTERRUPT_HANDLER(0xe8);
EVENT_INTERRUPT_HANDLER(0xe9);
EVENT_INTERRUPT_HANDLER(0xea);
EVENT_INTERRUPT_HANDLER(0xeb);
EVENT_INTERRUPT_HANDLER(0xec);
EVENT_INTERRUPT_HANDLER(0xed);
EVENT_INTERRUPT_HANDLER(0xee);
EVENT_INTERRUPT_HANDLER(0xef);
EVENT_INTERRUPT_HANDLER(0xf0);
EVENT_INTERRUPT_HANDLER(0xf1);
EVENT_INTERRUPT_HANDLER(0xf2);
EVENT_INTERRUPT_HANDLER(0xf3);
EVENT_INTERRUPT_HANDLER(0xf4);
EVENT_INTERRUPT_HANDLER(0xf5);
EVENT_INTERRUPT_HANDLER(0xf6);
EVENT_INTERRUPT_HANDLER(0xf7);
EVENT_INTERRUPT_HANDLER(0xf8);
EVENT_INTERRUPT_HANDLER(0xf9);
EVENT_INTERRUPT_HANDLER(0xfa);
EVENT_INTERRUPT_HANDLER(0xfb);
EVENT_INTERRUPT_HANDLER(0xfc);
EVENT_INTERRUPT_HANDLER(0xfd);
EVENT_INTERRUPT_HANDLER(0xfe);
EVENT_INTERRUPT_HANDLER(0xff);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define IDT structures and set the idt up
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IDT_TYPE_TASK           0x5
#define IDT_TYPE_INTERRUPT_16   0x6
#define IDT_TYPE_TRAP_16        0x7
#define IDT_TYPE_INTERRUPT_32   0xE
#define IDT_TYPE_TRAP_32        0xF

typedef struct idt_entry {
    uint64_t handler_low : 16;
    uint64_t selector : 16;
    uint64_t ist : 3;
    uint64_t _zero1 : 5;
    uint64_t gate_type : 4;
    uint64_t _zero2 : 1;
    uint64_t ring : 2;
    uint64_t present : 1;
    uint64_t handler_high : 48;
    uint64_t _zero3 : 32;
} PACKED idt_entry_t;

typedef struct idt {
    uint16_t limit;
    idt_entry_t* base;
} PACKED idt_t;

/**
 * All interrupt handler entries
 */
static idt_entry_t g_idt_entries[0xFF + 1];

/**
 * The idt
 */
static idt_t g_idt = {
    .limit = sizeof(g_idt_entries) - 1,
    .base = g_idt_entries
};

static void set_idt_entry(uint8_t i, void(*handler)(), int ist) {
    g_idt_entries[i].handler_low = (uint16_t) ((uintptr_t)handler & 0xFFFF);
    g_idt_entries[i].handler_high = (uint64_t) ((uintptr_t)handler >> 16);
    g_idt_entries[i].gate_type = IDT_TYPE_INTERRUPT_32;
    g_idt_entries[i].selector = GDT_KERNEL_CODE;
    g_idt_entries[i].present = 1;
    g_idt_entries[i].ring = 0;
    g_idt_entries[i].ist = (uint64_t) ist;
}

err_t init_idt() {
    err_t err = NO_ERROR;

    // create all events
    for (int i = 0; i < ARRAY_LENGTH(g_interrupt_events); i++) {
        CHECK_AND_RETHROW(create_event(&g_interrupt_events[i]));
    }

    // setup full idt
//    set_idt_entry(0x0, interrupt_handle_0x0, 0);
//    set_idt_entry(0x1, interrupt_handle_0x1, 0);
//    set_idt_entry(0x2, interrupt_handle_0x2, 0);
//    set_idt_entry(0x3, interrupt_handle_0x3, 0);
//    set_idt_entry(0x4, interrupt_handle_0x4, 0);
//    set_idt_entry(0x5, interrupt_handle_0x5, 0);
//    set_idt_entry(0x6, interrupt_handle_0x6, 0);
//    set_idt_entry(0x7, interrupt_handle_0x7, 0);
//    set_idt_entry(0x8, interrupt_handle_0x8, 0);
//    set_idt_entry(0x9, interrupt_handle_0x9, 0);
//    set_idt_entry(0xa, interrupt_handle_0xa, 0);
//    set_idt_entry(0xb, interrupt_handle_0xb, 0);
//    set_idt_entry(0xc, interrupt_handle_0xc, 0);
//    set_idt_entry(0xd, interrupt_handle_0xd, 0);
//    set_idt_entry(0xe, interrupt_handle_0xe, 0);
//    set_idt_entry(0xf, interrupt_handle_0xf, 0);
//    set_idt_entry(0x10, interrupt_handle_0x10, 0);
//    set_idt_entry(0x11, interrupt_handle_0x11, 0);
//    set_idt_entry(0x12, interrupt_handle_0x12, 0);
//    set_idt_entry(0x13, interrupt_handle_0x13, 0);
//    set_idt_entry(0x14, interrupt_handle_0x14, 0);
//    set_idt_entry(0x15, interrupt_handle_0x15, 0);
//    set_idt_entry(0x16, interrupt_handle_0x16, 0);
//    set_idt_entry(0x17, interrupt_handle_0x17, 0);
//    set_idt_entry(0x18, interrupt_handle_0x18, 0);
//    set_idt_entry(0x19, interrupt_handle_0x19, 0);
//    set_idt_entry(0x1a, interrupt_handle_0x1a, 0);
//    set_idt_entry(0x1b, interrupt_handle_0x1b, 0);
//    set_idt_entry(0x1c, interrupt_handle_0x1c, 0);
//    set_idt_entry(0x1d, interrupt_handle_0x1d, 0);
//    set_idt_entry(0x1e, interrupt_handle_0x1e, 0);
//    set_idt_entry(0x1f, interrupt_handle_0x1f, 0);
//    set_idt_entry(0x20, interrupt_handle_0x20, 0);
    set_idt_entry(0x21, interrupt_handle_0x21, 0);
    set_idt_entry(0x22, interrupt_handle_0x22, 0);
    set_idt_entry(0x23, interrupt_handle_0x23, 0);
    set_idt_entry(0x24, interrupt_handle_0x24, 0);
    set_idt_entry(0x25, interrupt_handle_0x25, 0);
    set_idt_entry(0x26, interrupt_handle_0x26, 0);
    set_idt_entry(0x27, interrupt_handle_0x27, 0);
    set_idt_entry(0x28, interrupt_handle_0x28, 0);
    set_idt_entry(0x29, interrupt_handle_0x29, 0);
    set_idt_entry(0x2a, interrupt_handle_0x2a, 0);
    set_idt_entry(0x2b, interrupt_handle_0x2b, 0);
    set_idt_entry(0x2c, interrupt_handle_0x2c, 0);
    set_idt_entry(0x2d, interrupt_handle_0x2d, 0);
    set_idt_entry(0x2e, interrupt_handle_0x2e, 0);
    set_idt_entry(0x2f, interrupt_handle_0x2f, 0);
    set_idt_entry(0x30, interrupt_handle_0x30, 0);
    set_idt_entry(0x31, interrupt_handle_0x31, 0);
    set_idt_entry(0x32, interrupt_handle_0x32, 0);
    set_idt_entry(0x33, interrupt_handle_0x33, 0);
    set_idt_entry(0x34, interrupt_handle_0x34, 0);
    set_idt_entry(0x35, interrupt_handle_0x35, 0);
    set_idt_entry(0x36, interrupt_handle_0x36, 0);
    set_idt_entry(0x37, interrupt_handle_0x37, 0);
    set_idt_entry(0x38, interrupt_handle_0x38, 0);
    set_idt_entry(0x39, interrupt_handle_0x39, 0);
    set_idt_entry(0x3a, interrupt_handle_0x3a, 0);
    set_idt_entry(0x3b, interrupt_handle_0x3b, 0);
    set_idt_entry(0x3c, interrupt_handle_0x3c, 0);
    set_idt_entry(0x3d, interrupt_handle_0x3d, 0);
    set_idt_entry(0x3e, interrupt_handle_0x3e, 0);
    set_idt_entry(0x3f, interrupt_handle_0x3f, 0);
    set_idt_entry(0x40, interrupt_handle_0x40, 0);
    set_idt_entry(0x41, interrupt_handle_0x41, 0);
    set_idt_entry(0x42, interrupt_handle_0x42, 0);
    set_idt_entry(0x43, interrupt_handle_0x43, 0);
    set_idt_entry(0x44, interrupt_handle_0x44, 0);
    set_idt_entry(0x45, interrupt_handle_0x45, 0);
    set_idt_entry(0x46, interrupt_handle_0x46, 0);
    set_idt_entry(0x47, interrupt_handle_0x47, 0);
    set_idt_entry(0x48, interrupt_handle_0x48, 0);
    set_idt_entry(0x49, interrupt_handle_0x49, 0);
    set_idt_entry(0x4a, interrupt_handle_0x4a, 0);
    set_idt_entry(0x4b, interrupt_handle_0x4b, 0);
    set_idt_entry(0x4c, interrupt_handle_0x4c, 0);
    set_idt_entry(0x4d, interrupt_handle_0x4d, 0);
    set_idt_entry(0x4e, interrupt_handle_0x4e, 0);
    set_idt_entry(0x4f, interrupt_handle_0x4f, 0);
    set_idt_entry(0x50, interrupt_handle_0x50, 0);
    set_idt_entry(0x51, interrupt_handle_0x51, 0);
    set_idt_entry(0x52, interrupt_handle_0x52, 0);
    set_idt_entry(0x53, interrupt_handle_0x53, 0);
    set_idt_entry(0x54, interrupt_handle_0x54, 0);
    set_idt_entry(0x55, interrupt_handle_0x55, 0);
    set_idt_entry(0x56, interrupt_handle_0x56, 0);
    set_idt_entry(0x57, interrupt_handle_0x57, 0);
    set_idt_entry(0x58, interrupt_handle_0x58, 0);
    set_idt_entry(0x59, interrupt_handle_0x59, 0);
    set_idt_entry(0x5a, interrupt_handle_0x5a, 0);
    set_idt_entry(0x5b, interrupt_handle_0x5b, 0);
    set_idt_entry(0x5c, interrupt_handle_0x5c, 0);
    set_idt_entry(0x5d, interrupt_handle_0x5d, 0);
    set_idt_entry(0x5e, interrupt_handle_0x5e, 0);
    set_idt_entry(0x5f, interrupt_handle_0x5f, 0);
    set_idt_entry(0x60, interrupt_handle_0x60, 0);
    set_idt_entry(0x61, interrupt_handle_0x61, 0);
    set_idt_entry(0x62, interrupt_handle_0x62, 0);
    set_idt_entry(0x63, interrupt_handle_0x63, 0);
    set_idt_entry(0x64, interrupt_handle_0x64, 0);
    set_idt_entry(0x65, interrupt_handle_0x65, 0);
    set_idt_entry(0x66, interrupt_handle_0x66, 0);
    set_idt_entry(0x67, interrupt_handle_0x67, 0);
    set_idt_entry(0x68, interrupt_handle_0x68, 0);
    set_idt_entry(0x69, interrupt_handle_0x69, 0);
    set_idt_entry(0x6a, interrupt_handle_0x6a, 0);
    set_idt_entry(0x6b, interrupt_handle_0x6b, 0);
    set_idt_entry(0x6c, interrupt_handle_0x6c, 0);
    set_idt_entry(0x6d, interrupt_handle_0x6d, 0);
    set_idt_entry(0x6e, interrupt_handle_0x6e, 0);
    set_idt_entry(0x6f, interrupt_handle_0x6f, 0);
    set_idt_entry(0x70, interrupt_handle_0x70, 0);
    set_idt_entry(0x71, interrupt_handle_0x71, 0);
    set_idt_entry(0x72, interrupt_handle_0x72, 0);
    set_idt_entry(0x73, interrupt_handle_0x73, 0);
    set_idt_entry(0x74, interrupt_handle_0x74, 0);
    set_idt_entry(0x75, interrupt_handle_0x75, 0);
    set_idt_entry(0x76, interrupt_handle_0x76, 0);
    set_idt_entry(0x77, interrupt_handle_0x77, 0);
    set_idt_entry(0x78, interrupt_handle_0x78, 0);
    set_idt_entry(0x79, interrupt_handle_0x79, 0);
    set_idt_entry(0x7a, interrupt_handle_0x7a, 0);
    set_idt_entry(0x7b, interrupt_handle_0x7b, 0);
    set_idt_entry(0x7c, interrupt_handle_0x7c, 0);
    set_idt_entry(0x7d, interrupt_handle_0x7d, 0);
    set_idt_entry(0x7e, interrupt_handle_0x7e, 0);
    set_idt_entry(0x7f, interrupt_handle_0x7f, 0);
    set_idt_entry(0x80, interrupt_handle_0x80, 0);
    set_idt_entry(0x81, interrupt_handle_0x81, 0);
    set_idt_entry(0x82, interrupt_handle_0x82, 0);
    set_idt_entry(0x83, interrupt_handle_0x83, 0);
    set_idt_entry(0x84, interrupt_handle_0x84, 0);
    set_idt_entry(0x85, interrupt_handle_0x85, 0);
    set_idt_entry(0x86, interrupt_handle_0x86, 0);
    set_idt_entry(0x87, interrupt_handle_0x87, 0);
    set_idt_entry(0x88, interrupt_handle_0x88, 0);
    set_idt_entry(0x89, interrupt_handle_0x89, 0);
    set_idt_entry(0x8a, interrupt_handle_0x8a, 0);
    set_idt_entry(0x8b, interrupt_handle_0x8b, 0);
    set_idt_entry(0x8c, interrupt_handle_0x8c, 0);
    set_idt_entry(0x8d, interrupt_handle_0x8d, 0);
    set_idt_entry(0x8e, interrupt_handle_0x8e, 0);
    set_idt_entry(0x8f, interrupt_handle_0x8f, 0);
    set_idt_entry(0x90, interrupt_handle_0x90, 0);
    set_idt_entry(0x91, interrupt_handle_0x91, 0);
    set_idt_entry(0x92, interrupt_handle_0x92, 0);
    set_idt_entry(0x93, interrupt_handle_0x93, 0);
    set_idt_entry(0x94, interrupt_handle_0x94, 0);
    set_idt_entry(0x95, interrupt_handle_0x95, 0);
    set_idt_entry(0x96, interrupt_handle_0x96, 0);
    set_idt_entry(0x97, interrupt_handle_0x97, 0);
    set_idt_entry(0x98, interrupt_handle_0x98, 0);
    set_idt_entry(0x99, interrupt_handle_0x99, 0);
    set_idt_entry(0x9a, interrupt_handle_0x9a, 0);
    set_idt_entry(0x9b, interrupt_handle_0x9b, 0);
    set_idt_entry(0x9c, interrupt_handle_0x9c, 0);
    set_idt_entry(0x9d, interrupt_handle_0x9d, 0);
    set_idt_entry(0x9e, interrupt_handle_0x9e, 0);
    set_idt_entry(0x9f, interrupt_handle_0x9f, 0);
    set_idt_entry(0xa0, interrupt_handle_0xa0, 0);
    set_idt_entry(0xa1, interrupt_handle_0xa1, 0);
    set_idt_entry(0xa2, interrupt_handle_0xa2, 0);
    set_idt_entry(0xa3, interrupt_handle_0xa3, 0);
    set_idt_entry(0xa4, interrupt_handle_0xa4, 0);
    set_idt_entry(0xa5, interrupt_handle_0xa5, 0);
    set_idt_entry(0xa6, interrupt_handle_0xa6, 0);
    set_idt_entry(0xa7, interrupt_handle_0xa7, 0);
    set_idt_entry(0xa8, interrupt_handle_0xa8, 0);
    set_idt_entry(0xa9, interrupt_handle_0xa9, 0);
    set_idt_entry(0xaa, interrupt_handle_0xaa, 0);
    set_idt_entry(0xab, interrupt_handle_0xab, 0);
    set_idt_entry(0xac, interrupt_handle_0xac, 0);
    set_idt_entry(0xad, interrupt_handle_0xad, 0);
    set_idt_entry(0xae, interrupt_handle_0xae, 0);
    set_idt_entry(0xaf, interrupt_handle_0xaf, 0);
    set_idt_entry(0xb0, interrupt_handle_0xb0, 0);
    set_idt_entry(0xb1, interrupt_handle_0xb1, 0);
    set_idt_entry(0xb2, interrupt_handle_0xb2, 0);
    set_idt_entry(0xb3, interrupt_handle_0xb3, 0);
    set_idt_entry(0xb4, interrupt_handle_0xb4, 0);
    set_idt_entry(0xb5, interrupt_handle_0xb5, 0);
    set_idt_entry(0xb6, interrupt_handle_0xb6, 0);
    set_idt_entry(0xb7, interrupt_handle_0xb7, 0);
    set_idt_entry(0xb8, interrupt_handle_0xb8, 0);
    set_idt_entry(0xb9, interrupt_handle_0xb9, 0);
    set_idt_entry(0xba, interrupt_handle_0xba, 0);
    set_idt_entry(0xbb, interrupt_handle_0xbb, 0);
    set_idt_entry(0xbc, interrupt_handle_0xbc, 0);
    set_idt_entry(0xbd, interrupt_handle_0xbd, 0);
    set_idt_entry(0xbe, interrupt_handle_0xbe, 0);
    set_idt_entry(0xbf, interrupt_handle_0xbf, 0);
    set_idt_entry(0xc0, interrupt_handle_0xc0, 0);
    set_idt_entry(0xc1, interrupt_handle_0xc1, 0);
    set_idt_entry(0xc2, interrupt_handle_0xc2, 0);
    set_idt_entry(0xc3, interrupt_handle_0xc3, 0);
    set_idt_entry(0xc4, interrupt_handle_0xc4, 0);
    set_idt_entry(0xc5, interrupt_handle_0xc5, 0);
    set_idt_entry(0xc6, interrupt_handle_0xc6, 0);
    set_idt_entry(0xc7, interrupt_handle_0xc7, 0);
    set_idt_entry(0xc8, interrupt_handle_0xc8, 0);
    set_idt_entry(0xc9, interrupt_handle_0xc9, 0);
    set_idt_entry(0xca, interrupt_handle_0xca, 0);
    set_idt_entry(0xcb, interrupt_handle_0xcb, 0);
    set_idt_entry(0xcc, interrupt_handle_0xcc, 0);
    set_idt_entry(0xcd, interrupt_handle_0xcd, 0);
    set_idt_entry(0xce, interrupt_handle_0xce, 0);
    set_idt_entry(0xcf, interrupt_handle_0xcf, 0);
    set_idt_entry(0xd0, interrupt_handle_0xd0, 0);
    set_idt_entry(0xd1, interrupt_handle_0xd1, 0);
    set_idt_entry(0xd2, interrupt_handle_0xd2, 0);
    set_idt_entry(0xd3, interrupt_handle_0xd3, 0);
    set_idt_entry(0xd4, interrupt_handle_0xd4, 0);
    set_idt_entry(0xd5, interrupt_handle_0xd5, 0);
    set_idt_entry(0xd6, interrupt_handle_0xd6, 0);
    set_idt_entry(0xd7, interrupt_handle_0xd7, 0);
    set_idt_entry(0xd8, interrupt_handle_0xd8, 0);
    set_idt_entry(0xd9, interrupt_handle_0xd9, 0);
    set_idt_entry(0xda, interrupt_handle_0xda, 0);
    set_idt_entry(0xdb, interrupt_handle_0xdb, 0);
    set_idt_entry(0xdc, interrupt_handle_0xdc, 0);
    set_idt_entry(0xdd, interrupt_handle_0xdd, 0);
    set_idt_entry(0xde, interrupt_handle_0xde, 0);
    set_idt_entry(0xdf, interrupt_handle_0xdf, 0);
    set_idt_entry(0xe0, interrupt_handle_0xe0, 0);
    set_idt_entry(0xe1, interrupt_handle_0xe1, 0);
    set_idt_entry(0xe2, interrupt_handle_0xe2, 0);
    set_idt_entry(0xe3, interrupt_handle_0xe3, 0);
    set_idt_entry(0xe4, interrupt_handle_0xe4, 0);
    set_idt_entry(0xe5, interrupt_handle_0xe5, 0);
    set_idt_entry(0xe6, interrupt_handle_0xe6, 0);
    set_idt_entry(0xe7, interrupt_handle_0xe7, 0);
    set_idt_entry(0xe8, interrupt_handle_0xe8, 0);
    set_idt_entry(0xe9, interrupt_handle_0xe9, 0);
    set_idt_entry(0xea, interrupt_handle_0xea, 0);
    set_idt_entry(0xeb, interrupt_handle_0xeb, 0);
    set_idt_entry(0xec, interrupt_handle_0xec, 0);
    set_idt_entry(0xed, interrupt_handle_0xed, 0);
    set_idt_entry(0xee, interrupt_handle_0xee, 0);
    set_idt_entry(0xef, interrupt_handle_0xef, 0);
    set_idt_entry(0xf0, interrupt_handle_0xf0, 0);
    set_idt_entry(0xf1, interrupt_handle_0xf1, 0);
    set_idt_entry(0xf2, interrupt_handle_0xf2, 0);
    set_idt_entry(0xf3, interrupt_handle_0xf3, 0);
    set_idt_entry(0xf4, interrupt_handle_0xf4, 0);
    set_idt_entry(0xf5, interrupt_handle_0xf5, 0);
    set_idt_entry(0xf6, interrupt_handle_0xf6, 0);
    set_idt_entry(0xf7, interrupt_handle_0xf7, 0);
    set_idt_entry(0xf8, interrupt_handle_0xf8, 0);
    set_idt_entry(0xf9, interrupt_handle_0xf9, 0);
    set_idt_entry(0xfa, interrupt_handle_0xfa, 0);
    set_idt_entry(0xfb, interrupt_handle_0xfb, 0);
    set_idt_entry(0xfc, interrupt_handle_0xfc, 0);
    set_idt_entry(0xfd, interrupt_handle_0xfd, 0);
    set_idt_entry(0xfe, interrupt_handle_0xfe, 0);
    set_idt_entry(0xff, interrupt_handle_0xfe, 0);

    // TODO: put as __lidt
    asm volatile ("lidt %0" : : "m" (g_idt));

cleanup:
    return err;
}
