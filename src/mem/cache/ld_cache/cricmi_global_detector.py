from m5.params import *
from m5.SimObject import SimObject


class CRICMIGlobalDetectorHack:
    pass

class CRICMIGlobalDetector(SimObject):
    type = "CRICMIGlobalDetector"
    cxx_class = "gem5::CRICMIGlobalDetector"
    cxx_header = "mem/cache/ld_cache/cricmi_global_detector.hh"


    # Parameters for detection thresholds
    thresholds = VectorParam.Int([10,10,10,10], "Threshold for interference events")
    decay_rate = Param.Int(1, "Rate of decay for counters")
    increase_rate = Param.Int(1, "Rate of increase for counters")
    frequency_threshold = Param.Int(5, "frequency_threshold")
    bucket_frequencies = VectorParam.Int([0] * 4, "Threshold for interference events")
    last_occurrence = VectorParam.Int([0] * 4, "Threshold for interference events")
    mapper_id = Param.Int(0, "mapper id")

    # TODO: Ruiying needs this so it can be passed to the global detector
    # alert_callback = Param.SimObject(None)

    # port initialization
    cpu_side_port = ResponsePort("GD response port for connecting to the bus")
