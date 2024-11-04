from caches import (
    L1DCache,
    L1ICache,
    L2Cache,
)

import m5
from m5.objects import *
from m5.util import (
    addToPath,
    convert,
)

# Add the common scripts to the path
addToPath("../../")

# creating the system - init
system = System()

# setting the clock frequency for the system - we need to discuss and change this
# TODO
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = "1GHz"
system.clk_domain.voltage_domain = VoltageDomain()

# setting the memory mode
system.mem_mode = "timing"
system.mem_ranges = [AddrRange("512MB")]  # change this as per need too for us

# Create two RISC-V TimingSimpleCPU objects
system.cpu = [TimingSimpleCPU() for i in range(2)]

# Create L1 instruction and data caches for each CPU
for i in range(2):
    system.cpu[i].icache = L1ICache()
    system.cpu[i].dcache = L1DCache()
    system.cpu[i].icache.connectCPU(system.cpu[i])
    system.cpu[i].dcache.connectCPU(system.cpu[i])

# createing a bus for the L1 caches
system.l1bus = L2XBar()

# connecting these L1 caches to the L1 bus
for i in range(2):
    system.cpu[i].icache.connectBus(system.l1bus)
    system.cpu[i].dcache.connectBus(system.l1bus)

# makwe a shared L2 cache
system.l2cache = L2Cache()
system.l2cache.connectCPUSideBus(system.l1bus)

# Create a memory bus
system.membus = SystemXBar()

# Connecting this L2 cache to memory bus
system.l2cache.connectMemSideBus(system.membus)

# Create the interrupt controllers for the CPUs and connect to the membus
for cpu in system.cpu:
    cpu.createInterruptController()

# Connect the system up to the membus
system.system_port = system.membus.cpu_side_ports

# Create a DDR3 memory controller and connect it to the membus
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

# using a process for the hello world test program
binary = "tests/test-progs/hello/bin/riscv/linux/hello"
system.workload = SEWorkload.init_compatible(binary)
process = Process()
process.cmd = [binary]

# setting the workload for both the CPUs
for cpu in system.cpu:
    cpu.workload = process
    cpu.createThreads()

# Setting up the root SimObject and running the simulation
root = Root(full_system=False, system=system)

# Instantiationnnn
m5.instantiate()

# lessgoooo

print("Beginning simulation!")
exit_event = m5.simulate()
print(
    f"Exiting @ tick {m5.curTick()} because {exit_event.getCause()}"
)


# TODO - fix the warnings and build from here
