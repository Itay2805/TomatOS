#ifndef TOMATOS_TRACE_H
#define TOMATOS_TRACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "map.h"
#include "err.h"

// char and string tracing
void trace_char(char c);
void trace_string(const char* c);

// normal tracing
void trace_int8(int8_t val);
void trace_int16(int16_t val);
void trace_int32(int32_t val);
void trace_int64(int64_t val);
void trace_uint8(uint8_t val);
void trace_uint16(uint16_t val);
void trace_uint32(uint32_t val);
void trace_uint64(uint64_t val);

// boolean tracing
void trace_bool(bool val);

// error tracing
void trace_err(err_t err);

// hex tracing
typedef struct trace_hex {
    uint64_t value;
    uint8_t bytes;
} trace_hex_t;
#define HEX(val) ((trace_hex_t){ .value = val, .bytes = sizeof(val) })
void trace_hex(trace_hex_t hex);

typedef struct trace_size {
    size_t val;
} trace_size_t;
#define SIZE(num) ((trace_size_t){ .val = num })
void trace_size(trace_size_t hex);

// pointer tracing
void trace_ptr(void* ptr);

#define PRINT_VALUE(val) \
    _Generic((val), \
        char: trace_char, \
        const char*: trace_string, \
        char*: trace_string, \
        bool: trace_bool, \
        err_t: trace_err, \
        trace_hex_t: trace_hex, \
        trace_size_t: trace_size, \
        void*: trace_ptr, \
        int8_t: trace_int8, \
        int16_t: trace_int16, \
        int32_t: trace_int32, \
        int64_t: trace_int64, \
        uint8_t: trace_uint8, \
        uint16_t: trace_uint16, \
        uint32_t: trace_uint32, \
        uint64_t: trace_uint64, \
        unsigned long long: trace_uint64 \
    )(val);

#ifdef __IN_EDITOR__
    void __dummy_trace();
    #define PRINT(...) __dummy_trace(__VA_ARGS__)
#else
    #define PRINT(...) MAP(PRINT_VALUE, ## __VA_ARGS__)
#endif

#define DEBUG(...) PRINT("[?] ", ## __VA_ARGS__, "\n")
#define TRACE(...) PRINT("[*] ", ## __VA_ARGS__, "\n")
#define WARN(...) PRINT("[!] ", ## __VA_ARGS__, "\n")
#define ERROR(...) PRINT("[-] ", ## __VA_ARGS__, "\n")

#endif //TOMATOS_TRACE_H
