#ifndef __IO_H__
#define __IO_H__
/* basic IO copied from pintos */

#include "lib.h"

/* Predefined file handles. */
#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#define PRINTF_FORMAT(FMT, FIRST) __attribute__ ((format (printf, FMT, FIRST)))

/* Standard functions. */
int printf (const char *, ...) PRINTF_FORMAT (1, 2);
int snprintf (char *, size_t, const char *, ...) PRINTF_FORMAT (3, 4);
int vprintf (const char *, va_list) PRINTF_FORMAT (1, 0);
int vsnprintf (char *, size_t, const char *, va_list) PRINTF_FORMAT (3, 0);
int putchar (int);
int puts (const char *);

/* Nonstandard functions. */
void hex_dump (uintptr_t ofs, const void *, size_t size, bool ascii);
void print_human_readable_size (uint64_t sz);

/* Internal functions. */
void __vprintf (const char *format, va_list args,
                void (*output) (char, void *), void *aux);
void __printf (const char *format,
               void (*output) (char, void *), void *aux, ...);

/* Try to be helpful. */
#define sprintf dont_use_sprintf_use_snprintf
#define vsprintf dont_use_vsprintf_use_vsnprintf

int hprintf (int, const char *, ...) PRINTF_FORMAT (2, 3);
int vhprintf (int, const char *, va_list) PRINTF_FORMAT (2, 0);

// ctype.h

static inline int islower (int c) { return c >= 'a' && c <= 'z'; }
static inline int isupper (int c) { return c >= 'A' && c <= 'Z'; }
static inline int isalpha (int c) { return islower (c) || isupper (c); }
static inline int isdigit (int c) { return c >= '0' && c <= '9'; }
static inline int isalnum (int c) { return isalpha (c) || isdigit (c); }
static inline int isxdigit (int c) {
  return isdigit (c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
static inline int isspace (int c) {
  return (c == ' ' || c == '\f' || c == '\n'
          || c == '\r' || c == '\t' || c == '\v');
}
static inline int isblank (int c) { return c == ' ' || c == '\t'; }
static inline int isgraph (int c) { return c > 32 && c < 127; }
static inline int isprint (int c) { return c >= 32 && c < 127; }
static inline int iscntrl (int c) { return (c >= 0 && c < 32) || c == 127; }
static inline int isascii (int c) { return c >= 0 && c < 128; }
static inline int ispunct (int c) {
  return isprint (c) && !isalnum (c) && !isspace (c);
}

static inline int tolower (int c) { return isupper (c) ? c - 'A' + 'a' : c; }
static inline int toupper (int c) { return islower (c) ? c - 'a' + 'A' : c; }

#endif
