#include "cricmi_local_detector.hh"
#include <iostream>

namespace gem5 {

CRICMILocalDetector::CRICMILocalDetector(const CRICMILocalDetectorParams &params)
    : SimObject(params),
      cpu_side_port("cpu_side_port", this),
      threshold(params.threshold),
      num_buckets(params.num_buckets),
      interval_limit(params.interval_limit),
      mapper_id(params.mapper_id)
{
    // Initialize internal data structures or logging if needed
}

// Constructor for CRICMICpuSidePort
CRICMILocalDetector::CRICMICpuSidePort::CRICMICpuSidePort(
    const std::string &name, CRICMILocalDetector *detector)
    : RequestPort(name, detector), detector(detector) {}


void CRICMILocalDetector::sendData(uint8_t bucket_idx, uint32_t event_history, char detector_type) {
    // Create a request and packet
    gem5::Addr dummyAddr = 0x00; // might use dummy addr, don't worry about it
    BucketInfo data;
    data.bucket_idx = bucket_idx;
    data.event_history = event_history;
    data.detector_type = detector_type;
    auto req = std::make_shared<gem5::Request>(dummyAddr, sizeof(data), gem5::Request::Flags(), 0);

    auto pkt = new gem5::Packet(req, gem5::MemCmd::WriteReq);
    pkt->dataDynamic(data);

    // Send the packet downstream
    if (!cpu_side_port.sendTimingReq(pkt)) {
        panic("Failed to send request to bus.");
    }
}


} // namespace gem5


