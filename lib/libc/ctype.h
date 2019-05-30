#ifndef TOMATKERNEL_CTYPE_H
#define TOMATKERNEL_CTYPE_H

static inline int isctrl(int c) { return (0 <= c && c <= 0x1F) || c == 0x7F; }
static inline int isblank(int c) { return c == '\t' || c == ' '; }
static inline int islower(int c) { return ('a' <= c && c <= 'z'); }
static inline int isupper(int c) { return ('A' <= c && c <= 'Z'); }
static inline int isalpha(int c) { return islower(c) || isupper(c); }
static inline int isdigit(int c) { return '0' <= c && c <= '9'; }
static inline int isalnum(int c) { return isalpha(c) || isdigit(c); }
static inline int isspace(int c) { return ('\t' <= c && c <= '\r') || c == ' '; }
static inline int isprint(int c) { return ' ' <= c && c <= '~'; }
static inline int isxdigit(int c) { return isdigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }
static inline int ispunct(int c) { return ('!' <= c && c <= '/') || (':' <= c && c <= '@') || ('[' <= c && c <= '`') || ('{' <= c && c <= '~'); };
static inline int tolower(int c) { return isupper(c) ? c + 0x20 : c; }
static inline int toupper(int c) { return islower(c) ? c - 0x20 : c; }

#endif //TOMATKERNEL_CTYPE_H
