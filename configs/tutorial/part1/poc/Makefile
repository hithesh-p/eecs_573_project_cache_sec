.PHONY: all clean objdump-attacker objdump-victim

LIBS=lib.c io.c str.c
HEADERS=lib.h io.h str.h
# CFLAGS=-O2
CFLAGS=-std=gnu11 -ffunction-sections -Wl,--gc-sections

all: dockcross-riscv attacker victim

%.o: %.c
	./dockcross-riscv bash -c '$$CC -c $^ -o $@ -static -nostdlib $(CFLAGS)'

str.o: str.c
	./dockcross-riscv bash -c '$$CC -c $^ -o $@ -static -nostdlib -O2 $(CFLAGS)'

libs.a: $(patsubst %.c,%.o,$(LIBS))
	ar rcs $@ $^

dockcross-riscv: Makefile
	docker run --rm dockcross/linux-riscv64 > ./dockcross-riscv
	chmod +x ./dockcross-riscv

attacker: attacker.c libs.a $(HEADERS) dockcross-riscv Makefile
	./dockcross-riscv bash -c '$$CC attacker.c libs.a -o attacker -static -nostdlib $(CFLAGS)'

victim: victim.c libs.a $(HEADERS) dockcross-riscv Makefile
	./dockcross-riscv bash -c '$$CC victim.c libs.a -o victim -static -nostdlib $(CFLAGS)'

objdump-attacker:
	./dockcross-riscv bash -c 'riscv64-unknown-linux-gnu-objdump -d attacker'

objdump-victim:
	./dockcross-riscv bash -c 'riscv64-unknown-linux-gnu-objdump -d victim'

clean:
	rm -f attacker victim *.o
