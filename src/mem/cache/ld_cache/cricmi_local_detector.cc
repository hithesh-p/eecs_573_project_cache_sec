#include "cricmi_local_detector.hh"

namespace gem5 {

CRICMILocalDetector::CRICMILocalDetector(const CRICMILocalDetectorParams &params)
    : SimObject(params),
      threshold(params.threshold),
      num_buckets(params.num_buckets),
      interval_limit(params.interval_limit),
      mapper_id(params.mapper_id)
{
    // Initialize internal data structures or logging if needed
}

} // namespace gem5
