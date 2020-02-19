#ifndef __UTIL_TRACE_H__
#define __UTIL_TRACE_H__

void trace(const char* fmt, ...);

#define TRACE(fmt, ...) trace("[*] " fmt "\n", ## __VA_ARGS__)

#endif //__UTIL_TRACE_H__
