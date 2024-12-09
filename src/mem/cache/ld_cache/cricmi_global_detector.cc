#include "cricmi_global_detector.hh"
#include "base/debug.hh"
#include <mutex>

#include <iostream>

namespace gem5 {

CRICMIGlobalDetector *CRICMIGlobalDetector::hack_global_detector = nullptr;

CRICMIGlobalDetector::CRICMIGlobalDetector(const CRICMIGlobalDetectorParams &params)
    : SimObject(params),
      cpu_side_port(params.name + ".cpu_side_port", this),
      decay_rate(params.decay_rate),
      increase_rate(params.increase_rate),
      frequency_threshold(params.frequency_threshold),
      thresholds(params.thresholds),
      bucket_frequencies(params.bucket_frequencies),
      last_occurrence(params.last_occurrence),
      mapper_id(params.mapper_id)
{
    if (hack_global_detector)
        fatal("Error: hack_global_detector already exists\n");
    hack_global_detector = this;
}

// Constructor for CRICMICPUSidePort
CRICMIGlobalDetector::CRICMICPUSidePort::CRICMICPUSidePort(
    const std::string &name, CRICMIGlobalDetector *detector)
    : ResponsePort(name, detector), detector(detector) {}

// Handle incoming packets
bool CRICMIGlobalDetector::CRICMICPUSidePort::recvTimingReq(PacketPtr pkt) {
    if (pkt->isWrite()) {
        uint8_t *data = pkt->getPtr<uint8_t>();
        // Process the received data
        // DPRINTF(SimObject, "CRICMIGlobalDetector received write data: %d\n", *data);
        std::cout << "CRICMIGlobalDetector received write data: " << static_cast<int>(*data) << std::endl;
    } else if (pkt->isRead()) {
        // Handle read requests if necessary
        // DPRINTF(SimObject, "CRICMIGlobalDetector received a read request\n");
        std::cout << "CRICMIGlobalDetector received a read request" << std::endl;
    }

    // Indicate that the packet was successfully handled
    return true;
}

int CRICMIGlobalDetector::classifyAttack(int bucket_idx, int history_last) {
    static std::mutex m;
    std::unique_lock guard(m);

    int result = 0;

    for (size_t i = 0; i < bucket_frequencies.size(); ++i) {
        if (i == bucket_idx) {
            if (history_last > thresholds[bucket_idx]) { // event_History.back()
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

    if (result){
        std::cout<<"GD reported ATTACK!\n";
    } else {
        std::cout<<"GD reported NO Attack.\n";
    }

    return result;
}

Port &
CRICMIGlobalDetector::getPort(const std::string &if_name, PortID idx)
{
    if (if_name == "cpu_side_port") {
        return cpu_side_port;
    }  else {
        // return ClockedObject::getPort(if_name, idx);
        fatal("EECS573_HACK\n");
    }
}



} // namespace gem5
