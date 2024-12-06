#do not use this code, we will be using the one in two_cpu_riscv.py

# from caches import (
#     L1Cache,
#     L1DCache,
#     L1ICache,
# )

# import m5
# from m5.objects import *
# from m5.util import (
#     addToPath,
#     convert,
# )

# # Add the common scripts to the path
# addToPath("../../")

# # creating the system
# system = System()

# # set the clock freq for the system (we need to decide on this)
# system.clk_domain = SrcClockDomain()
# system.clk_domain.clock = "1GHz"
# system.clk_domain.voltage_domain = VoltageDomain()

# # mode of the CPU and address ranges
# system.mem_mode = "atomic"
# system.mem_ranges = [AddrRange("512MB")]

# # Create two RISC-V TimingSimpleCPU objects
# system.cpu = [TimingSimpleCPU() for i in range(2)]

# # creating a shared L1 cache
# system.l1cache = L1Cache()
# system.l1cache.size = (
#     "128kB"  # Increased size for shared cache (we need to decide on this too)
# )

# # Create an L1 cache bus to allow multiple CPUs to connect to the shared cache
# system.l1cache_bus = SystemXBar()


# # Connect each CPU’s instruction and data ports to the L1 cache bus
# for cpu in system.cpu:
#     cpu.icache_port = system.l1cache_bus.cpu_side_ports
#     cpu.dcache_port = system.l1cache_bus.cpu_side_ports

# # Connect the L1 cache bus to the shared L1 cache
# system.l1cache.connectBus(system.l1cache_bus)

# # memory bus
# system.membus = SystemXBar()

# # Now, connect the L1 cache bus to the main memory bus (instead of connecting l1cache directly)
# system.l1cache_bus.mem_side_ports = system.membus.cpu_side_ports

# # Create the interrupt controllers for the CPUs and connect to the membus
# for cpu in system.cpu:
#     cpu.createInterruptController()
#     # since RiscvInterrupts class in gem5 does not have a pio parameter
#     # cpu.interrupts[0].pio = system.membus.mem_side_ports
#     # cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
#     # cpu.interrupts[0].int_responder = system.membus.mem_side_ports

# # connecting the system up to the membus
# system.system_port = system.membus.cpu_side_ports

# # using a DDR3 memory controller and connecting it to the membus
# system.mem_ctrl = MemCtrl()
# system.mem_ctrl.dram = DDR3_1600_8x8()
# system.mem_ctrl.dram.range = system.mem_ranges[0]
# system.mem_ctrl.port = system.membus.mem_side_ports

# binary = "tests/test-progs/hello/bin/riscv/linux/hello"
# system.workload = SEWorkload.init_compatible(binary)
# # process for a simple "Hello World" as in the turorial
# process = Process()
# process.cmd = [binary]
# # system.cpu[0].workload = process
# # system.cpu[0].createThreads()
# # assigning work to both cpus just in case
# for cpu in system.cpu:
#     cpu.workload = process
#     cpu.createThreads()

# # Set up the root SimObject and start the simulation
# root = Root(full_system=False, system=system)

# # Instantiate system
# m5.instantiate()

# print("Beginning simulation!")
# exit_event = m5.simulate()
# print(
#     f"Exiting @ tick {m5.curTick()} because {exit_event.getCause()}"
# )
