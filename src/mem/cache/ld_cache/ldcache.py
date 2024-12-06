from m5.objects import Cache, SimObject, Param

class L1Cache(Cache):
    assoc = 2
    tag_latency = 2
    data_latency = 2
    response_latency = 2
    mshrs = 4
    tgts_per_mshr = 20

    # def __init__(self, detector=None, options=None):
    #     super().__init__()
    #     self.detector = detector  # Local detector instance


    def connectCPU(self, cpu):
        # need to define this in a base class!
        raise NotImplementedError

    def connectBus(self, bus):
        self.mem_side = bus.cpu_side_ports


    def access(self, previous_domain, request_domain, current_domain):
        """
        Trigger the detector on access events.
        """
        if self.detector:
            self.detector.simulate_access(
                previous_domain=previous_domain,
                request_domain=request_domain,
                current_domain=current_domain,
            )
        else:
            print(f"No detector connected for cache access: prev={previous_domain}, req={request_domain}, curr={current_domain}")


class L1ICache(L1Cache):
    size = "16kB"

    def connectCPU(self, cpu):
        self.cpu_side = cpu.icache_port

    # def __init__(self, detector=None, options=None):
    #     super().__init__(detector, options)
    #     if options and options.l1i_size:
    #         self.size = options.l1i_size


class L1DCache(L1Cache):
    size = "64kB"

    def connectCPU(self, cpu):
        self.cpu_side = cpu.dcache_port

    # def __init__(self, detector=None, options=None):
    #     super().__init__(detector, options)
    #     if options and options.l1d_size:
    #         self.size = options.l1d_size


class L2Cache(Cache):
    size = "256kB"
    assoc = 8
    tag_latency = 20
    data_latency = 20
    response_latency = 20
    mshrs = 20
    tgts_per_mshr = 12

    # def __init__(self, detector=None, options=None):
    #     super().__init__()
    #     self.detector = detector  # Local detector instance



    def connectCPUSideBus(self, bus):
        self.cpu_side = bus.mem_side_ports

    def connectMemSideBus(self, bus):
        self.mem_side = bus.cpu_side_ports


    def access(self, previous_domain, request_domain, current_domain):
        """
        Trigger the detector on access events.
        """
        if self.detector:
            self.detector.simulate_access(
                previous_domain=previous_domain,
                request_domain=request_domain,
                current_domain=current_domain,
            )

        else:
            print(f"No detector connected for cache access: prev={previous_domain}, req={request_domain}, curr={current_domain}")

