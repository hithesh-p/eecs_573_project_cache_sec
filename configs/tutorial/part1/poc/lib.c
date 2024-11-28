#include <stddef.h>
#include <stdint.h>

#include <time.h>

#include "lib.h"
#include "io.h"
#include "str.h"

ssize_t write(int fd, const void *buf, size_t count) {
	ssize_t res;

	__asm__(
	"mv a0, %1   \n"
	"mv a1, %2   \n"
	"mv a2, %3   \n"
	"li a7, 64   \n"
	"ecall \n"
	"mv %0, a0 \n"
	:"=r" (res)
	:"r"(fd), "r"(buf), "r"(count)
	:"memory");

	return res;
}

void exit(int ret) {
	__asm__(
	"mv a0, %0 \n"
	"li a7, 93 \n"
	"ecall \n"
	:
	:"r"(ret)
	);

	NOT_REACHED();
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
	int res;

	__asm__(
	"mv a0, %1   \n"
	"mv a1, %2   \n"
	"li a7, 101  \n"
	"ecall \n"
	"mv %0, a0 \n"
	:"=r" (res)
	:"r"(req), "r"(rem)
	:"memory");

	return res;
}

int clock_nanosleep(clockid_t clockid, int flags,
                           const struct timespec *t,
                           struct timespec *remain)
{
	int res;

	__asm__(
	"mv a0, %1   \n"
	"mv a1, %2   \n"
	"mv a2, %3   \n"
	"mv a3, %4   \n"
	"li a7, 115  \n"
	"ecall \n"
	"mv %0, a0 \n"
	:"=r" (res)
	:"r"(clockid), "r"(flags), "r"(t), "r"(remain)
	:"memory");

	return res;
}


extern int main();
// extern int main(int argc, char **argv, char **env);

NO_RETURN void _start(int argc, char **argv, char **env) {
	exit(main());
	// exit(main(argc, argv, env));
	// args does not work yet
}

NO_RETURN void panic(const char *msg) {
	char panicbuf[256] = "PANIC: ";
	strlcat(panicbuf, msg, 256);
	size_t len = strlcat(panicbuf, "\n", 256);
	write(1, panicbuf, len);
	exit(255);
}

// Does not work under gem5
// https://github.com/lattera/glibc/blob/master/sysdeps/unix/clock_nanosleep.c
// https://github.com/lattera/glibc/blob/master/sysdeps/unix/sysv/linux/clock_nanosleep.c
int
usleep (long useconds)
{
  struct timespec ts = { .tv_sec = (long int) (useconds / 1000000),
			 .tv_nsec = (long int) (useconds % 1000000) * 1000ul };

  return nanosleep (&ts, NULL);
  // return clock_nanosleep (CLOCK_MONOTONIC, 0, &ts, NULL);
}

void __attribute__((optimize("O0"))) busy_loop(unsigned long i) {
	while (i--) ;
}
