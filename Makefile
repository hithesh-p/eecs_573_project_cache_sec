.PHONY: all test

# CC=clang-15
# CXX=clang++15

all:
	scons build/RISCV/gem5.opt -j`nproc`

test:
	./build/RISCV/gem5.opt -v src/mem/cache/ld_cache/ld_two_cpu_riscv.py

test-local-unit:
	cd src/mem/cache/ld_cache/ && $(CXX) local_detector.cc -o local_detector -std=c++11 -pthread && ./local_detector

test-global-unit:
	python3 configs/design_codes_573/global_detector/test_global_detector.py
