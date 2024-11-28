#include "lib.h"
#include "io.h"
#include "str.h"

#define CACHELINE 64
#define PAGE 4096

char *secret = (char*)0x3fff0000UL;

uint64_t __attribute__((optimize("O0"))) measure_access_time(int index) {
	uint64_t start, end;

	// Inline assembly to read the RISC-V cycle counter
	asm volatile("rdcycle %0" : "=r"(start));
	secret[index * CACHELINE] = 0xfc;
	asm volatile("rdcycle %0" : "=r"(end));

	return end - start;
}

int main() {
	memset(secret, 0xbe, PAGE);
	printf("[1] victim: done init\n");

	busy_loop(2000000);
	printf("[4] victim: done spinning\n");

	uint64_t access_time = measure_access_time(54);

	printf("[5] victim access once; time %lu\n", access_time);

	printf("[6] victim: continue spinning\n");
	busy_loop(1000000);
	printf("[10] victim: spinning done\n");

	return 0;
}
