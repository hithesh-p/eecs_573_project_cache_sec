#include "cricmi_global_detector.hh"
#include "base/debug.hh"

#include <iostream>

namespace gem5 {


CRICMIGlobalDetector::CRICMIGlobalDetector(const CRICMIGlobalDetectorParams &params)
    : SimObject(params),
      memSidePort("mem_side_port", this),
      decay_rate(params.decay_rate),
      increase_rate(params.increase_rate),
      frequency_threshold(params.frequency_threshold),
      thresholds(params.thresholds),
      bucket_frequencies(params.bucket_frequencies),
      last_occurrence(params.last_occurrence),
      mapper_id(params.mapper_id) {}

// Constructor for CRICMIMemSidePort
CRICMIGlobalDetector::CRICMIMemSidePort::CRICMIMemSidePort(
    const std::string &name, CRICMIGlobalDetector *detector)
    : ResponsePort(name, detector), detector(detector) {}

// Handle incoming packets
bool CRICMIGlobalDetector::CRICMIMemSidePort::recvTimingReq(PacketPtr pkt) {
    if (pkt->isWrite()) {
        BucketInfo *data = pkt->getPtr<BucketInfo*>();
        bucket_idx = data->bucket_idx;
        detector_type = data->detector_type;
        this->classifyAttack(data->event_history);
        std::cout << "CRICMIGlobalDetector received data for bucket: " << data-> bucket_idx << std::endl;
    } else if (pkt->isRead()) {
        // Handle read requests if necessary
        // DPRINTF(SimObject, "CRICMIGlobalDetector received a read request\n");
        std::cout << "CRICMIGlobalDetector received a read request" << std::endl;
    }

    // Indicate that the packet was successfully handled
    return true;
}

int CRICMIGlobalDetector::classifyAttack(uint32_t event_history) {
    int result = 0;

    for (size_t i = 0; i < bucket_frequencies.size(); ++i) {
        if (i == bucket_idx) {
            if (event_history > thresholds[bucket_idx]) { // event_History.back()
                result = 1; // Attack detected
            }
            bucket_frequencies[bucket_idx]++;
            last_occurrence[bucket_idx] = 0;
        } else {
            last_occurrence[i]++;
        }
    }


    // Update thresholds
    for (size_t i = 0; i < thresholds.size(); ++i) {
        if (bucket_frequencies[i] >= frequency_threshold) {
            thresholds[i] = std::max(thresholds[i] - decay_rate, 0); // Reduce threshold, ensure non-negative
            bucket_frequencies[i] = 0;
        }
        if (last_occurrence[i] >= frequency_threshold) {
            thresholds[i] += increase_rate; // Increase threshold
            last_occurrence[i] = 0;
        }
    }

    return result;
}

} // namespace gem5