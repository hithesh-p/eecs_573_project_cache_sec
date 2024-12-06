#ifndef __CRICMI_LOCAL_DETECTOR_HH__
#define __CRICMI_LOCAL_DETECTOR_HH__

#include "params/CRICMILocalDetector.hh" // Auto-generated params
#include "sim/sim_object.hh"

namespace gem5 {

class CRICMILocalDetector : public SimObject {
public:
    CRICMILocalDetector(const CRICMILocalDetectorParams &params);

private:
    int threshold;
    int num_buckets;
    int interval_limit;
    int mapper_id;
};

} // namespace gem5

#endif // __CRICMI_LOCAL_DETECTOR_HH__
