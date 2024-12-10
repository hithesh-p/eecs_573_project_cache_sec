# EECS 573 Project: Side-Channel-Resilient Cache Design - Cache Security in RISC-V Systems

## To Run the code, please first refer to [Gem5 Getting Started](#getting-started), and then use the Makefile for our project tests:
      
      make (Runs the integration on gem5)
      make all (To build RISCV for gem5)
      make test (Runs the integration test on gem5)
      make test-local-unit (Runs the unittest for local detector)
      make test-global-unit (Runs the unittest for global detector)
      make test-newcache (Runs the unittest for newcache module)

## Overview

Our project explores cache security in RISC-V systems using the gem5 simulator. It's based on the gem5 simulator, a modular platform for computer-system architecture research, encompassing system-level architecture as well as processor microarchitecture.

## Credit and Source

This project is built upon the gem5 simulator. For more information about gem5, visit [gem5.org](https://www.gem5.org/).

## Getting Started

This repository contains a pre-built RISC-V version of gem5. If you've cloned this repo and checked out to my `eecs_573_proj_main` branch, you can use it directly to test the programs.

### Building gem5 (RISC-V version) from Scratch

If you prefer to build gem5 yourself, follow these steps:

1. Install dependencies (Ubuntu 24.04 or similar):
   ```bash
   sudo apt install build-essential scons python3-dev git pre-commit zlib1g zlib1g-dev \
   libprotobuf-dev protobuf-compiler libprotoc-dev libgoogle-perftools-dev \
   libboost-all-dev libhdf5-serial-dev python3-pydot python3-venv python3-tk mypy \
   m4 libcapstone-dev libpng-dev libelf-dev pkg-config wget cmake doxygen


2. Clone the repository:
   ```bash
   git clone https://github.com/hithesh-p/eecs_573_project_cache_sec.git
   cd eecs_573_project_cache_sec/gem5

3. Build gem5 with RISC-V support:
   ```bash
   scons build/RISCV/gem5.opt -j$(nproc)

This builds the optimized version of gem5 for RISC-V, using all available CPU cores.


### Project Structure
The following files in configs/tutorial/part1/ are custom implementations based on the gem5 official tutorial (for now):
   - caches.py
   - simple.py
   - two_level.py

These have been tested with the X86 build version of gem5.
Additional files:
   - two_cpu_risv.py: Prototype code for the basic system
   - two_cpu_riscv.py: Final code implementing the basic system


### System Architecture
The basic system implemented in two_cpu_riscv.py consists of:
1. Two CPUs (CPU 0 and CPU 1)
2. Each CPU has its own private L1 Instruction Cache (ICache) and L1 Data Cache (DCache)
3. Both L1 caches for each CPU are connected to a common L1 Bus (L2XBar)
4. A shared L2 Cache sits between the L1 Bus and the Memory Bus (SystemXBar)
5. The Memory Bus connects to a Memory Controller (MemCtrl), which interfaces with DDR3 Memory (DDR3_1600_8x8)

### System Architecture Diagram

             +------------------+       +------------------+
             |      CPU 0        |       |      CPU 1        |
             +------------------+       +------------------+
             |   ICache 0        |       |   ICache 1        |
             +------------------+       +------------------+
             |   DCache 0        |       |   DCache 1        |
             +------------------+       +------------------+
                      \                     /
                       \                   /
                        +----------------+
                        |     L1 Bus      |
                        +----------------+
                               |
                               v
                        +----------------+
                        |    L2 Cache     |
                        +----------------+
                               |
                               v
                        +----------------+
                        |   Memory Bus    |
                        +----------------+
                               |
                               v
                    +-----------------------+
                    |   Memory Controller    |
                    +-----------------------+
                               |
                               v
                    +-----------------------+
                    |     DDR3_1600_8x8     |
                    +-----------------------+


This serves as our baseline architecture. We shall all make our modifications and improvements to this design as we progress with the project.
Running Simulations
1. To run a simulation using the RISC-V version of gem5:
   ```bash
   ./build/RISCV/gem5.opt configs/tutorial/part1/two_cpu_riscv.py

### Contributing
Let us all use the baseline architecture as a starting point and propose changes or improvements to enhance cache security for our project in this RISC-V systems.

### License
This project is licensed under the terms of the gem5 license as below.




# The gem5 Simulator

This is the repository for the gem5 simulator. It contains the full source code
for the simulator and all tests and regressions.

The gem5 simulator is a modular platform for computer-system architecture
research, encompassing system-level architecture as well as processor
microarchitecture. It is primarily used to evaluate new hardware designs,
system software changes, and compile-time and run-time system optimizations.

The main website can be found at <http://www.gem5.org>.

## Testing status

**Note**: These regard tests run on the develop branch of gem5:
<https://github.com/gem5/gem5/tree/develop>.

[![Daily Tests](https://github.com/gem5/gem5/actions/workflows/daily-tests.yaml/badge.svg)](https://github.com/gem5/gem5/actions/workflows/daily-tests.yaml)
[![Weekly Tests](https://github.com/gem5/gem5/actions/workflows/weekly-tests.yaml/badge.svg)](https://github.com/gem5/gem5/actions/workflows/weekly-tests.yaml)
[![Compiler Tests](https://github.com/gem5/gem5/actions/workflows/compiler-tests.yaml/badge.svg)](https://github.com/gem5/gem5/actions/workflows/compiler-tests.yaml)

## Getting started

A good starting point is <http://www.gem5.org/about>, and for
more information about building the simulator and getting started
please see <http://www.gem5.org/documentation> and
<http://www.gem5.org/documentation/learning_gem5/introduction>.

## Building gem5

To build gem5, you will need the following software: g++ or clang,
Python (gem5 links in the Python interpreter), SCons, zlib, m4, and lastly
protobuf if you want trace capture and playback support. Please see
<http://www.gem5.org/documentation/general_docs/building> for more details
concerning the minimum versions of these tools.

Once you have all dependencies resolved, execute
`scons build/ALL/gem5.opt` to build an optimized version of the gem5 binary
(`gem5.opt`) containing all gem5 ISAs. If you only wish to compile gem5 to
include a single ISA, you can replace `ALL` with the name of the ISA. Valid
options include `ARM`, `NULL`, `MIPS`, `POWER`, `RISCV`, `SPARC`, and `X86`
The complete list of options can be found in the build_opts directory.

See https://www.gem5.org/documentation/general_docs/building for more
information on building gem5.

## The Source Tree

The main source tree includes these subdirectories:

* build_opts: pre-made default configurations for gem5
* build_tools: tools used internally by gem5's build process.
* configs: example simulation configuration scripts
* ext: less-common external packages needed to build gem5
* include: include files for use in other programs
* site_scons: modular components of the build system
* src: source code of the gem5 simulator. The C++ source, Python wrappers, and Python standard library are found in this directory.
* system: source for some optional system software for simulated systems
* tests: regression tests
* util: useful utility programs and files

## gem5 Resources

To run full-system simulations, you may need compiled system firmware, kernel
binaries and one or more disk images, depending on gem5's configuration and
what type of workload you're trying to run. Many of these resources can be
obtained from <https://resources.gem5.org>.

More information on gem5 Resources can be found at
<https://www.gem5.org/documentation/general_docs/gem5_resources/>.

## Getting Help, Reporting bugs, and Requesting Features

We provide a variety of channels for users and developers to get help, report
bugs, requests features, or engage in community discussions. Below
are a few of the most common we recommend using.

* **GitHub Discussions**: A GitHub Discussions page. This can be used to start
discussions or ask questions. Available at
<https://github.com/orgs/gem5/discussions>.
* **GitHub Issues**: A GitHub Issues page for reporting bugs or requesting
features. Available at <https://github.com/gem5/gem5/issues>.
* **Jira Issue Tracker**: A Jira Issue Tracker for reporting bugs or requesting
features. Available at <https://gem5.atlassian.net/>.
* **Slack**: A Slack server with a variety of channels for the gem5 community
to engage in a variety of discussions. Please visit
<https://www.gem5.org/join-slack> to join.
* **gem5-users@gem5.org**: A mailing list for users of gem5 to ask questions
or start discussions. To join the mailing list please visit
<https://www.gem5.org/mailing_lists>.
* **gem5-dev@gem5.org**: A mailing list for developers of gem5 to ask questions
or start discussions. To join the mailing list please visit
<https://www.gem5.org/mailing_lists>.

## Contributing to gem5

We hope you enjoy using gem5. When appropriate we advise charing your
contributions to the project. <https://www.gem5.org/contributing> can help you
get started. Additional information can be found in the CONTRIBUTING.md file.
