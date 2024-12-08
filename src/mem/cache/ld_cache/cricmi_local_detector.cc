#include "cricmi_local_detector.hh"
#include <iostream>

namespace gem5 {

CRICMILocalDetector::CRICMILocalDetector(const CRICMILocalDetectorParams &params)
    : SimObject(params),
      mem_side_port(params.name + ".mem_side_port", this),
      threshold(params.threshold),
      num_buckets(params.num_buckets),
      interval_limit(params.interval_limit),
      mapper_id(params.mapper_id)
{
    // Initialize internal data structures or logging if needed
}

// Constructor for CRICMIMemSidePort
CRICMILocalDetector::CRICMIMemSidePort::CRICMIMemSidePort(
    const std::string &name, CRICMILocalDetector *detector)
    : RequestPort(name, detector), detector(detector) {}


Port &
CRICMILocalDetector::getPort(const std::string &if_name, PortID idx)
{
    if (if_name == "mem_side_port") {
        return mem_side_port;
    }  else {
        // return ClockedObject::getPort(if_name, idx);
        fatal("EECS573_HACK\n");
    }
}


void CRICMILocalDetector::sendData(uint8_t *data) {
    // Create a request and packet
    gem5::Addr dummyAddr = 0x00; // might use dummy addr, don't worry about it
    auto req = std::make_shared<gem5::Request>(dummyAddr, sizeof(data), gem5::Request::Flags(), 0);

    auto pkt = new gem5::Packet(req, gem5::MemCmd::WriteReq);
    pkt->dataDynamic(data);

    // Send the packet downstream
    if (!mem_side_port.sendTimingReq(pkt)) {
        panic("Failed to send request to bus.");
    }
}


} // namespace gem5


