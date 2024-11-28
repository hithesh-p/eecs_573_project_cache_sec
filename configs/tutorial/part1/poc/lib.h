#ifndef __LIB_H__
#define __LIB_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>

#define UNUSED __attribute__ ((unused))
#define NO_RETURN __attribute__ ((noreturn))
#define NO_INLINE __attribute__ ((noinline))

#define NOT_REACHED() do { panic("not reached"); } while (0)

#define ASSERT(CONDITION)                                       \
        if (CONDITION) { } else {                               \
                panic ("assertion `" #CONDITION "' failed.");   \
        }

typedef __builtin_va_list va_list;

#define va_start(LIST, ARG)	__builtin_va_start (LIST, ARG)
#define va_end(LIST)            __builtin_va_end (LIST)
#define va_arg(LIST, TYPE)	__builtin_va_arg (LIST, TYPE)
#define va_copy(DST, SRC)	__builtin_va_copy (DST, SRC)

/* Yields X rounded up to the nearest multiple of STEP.
   For X >= 0, STEP >= 1 only. */
#define ROUND_UP(X, STEP) (((X) + (STEP) - 1) / (STEP) * (STEP))

/* Yields X divided by STEP, rounded up.
   For X >= 0, STEP >= 1 only. */
#define DIV_ROUND_UP(X, STEP) (((X) + (STEP) - 1) / (STEP))

/* Yields X rounded down to the nearest multiple of STEP.
   For X >= 0, STEP >= 1 only. */
#define ROUND_DOWN(X, STEP) ((X) / (STEP) * (STEP))

int nanosleep(const struct timespec *req, struct timespec *rem);
ssize_t write(int fd, const void *buf, size_t count);
void exit(int ret) NO_RETURN;

int usleep(long int useconds);
void panic(const char *msg);
void busy_loop(unsigned long i);

#endif
