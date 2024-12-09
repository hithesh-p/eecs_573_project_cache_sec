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
    event_counters.resize(num_buckets, 0);
    event_histories.resize(num_buckets, std::vector<int>(8, 0));
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


void CRICMILocalDetector::detectCyclicInterference(int previous_domain, int request_domain, int current_domain) {
        auto& states = *hack;

        for (int i = 1; i <= num_buckets; ++i) {
            if (previous_domain == i && request_domain == i && request_domain != current_domain) {
                states.event_counters[i - 1] = (states.event_counters[i - 1] + 1) % 256;

                if (states.event_counters[i - 1] >= threshold) {
                    raiseAlert(i);
                    states.event_counters[i - 1] = 0;
                }

                updateEventHistories(i - 1);
            }
        }
    }

    void CRICMILocalDetector::updateEventHistories(int bucket_index) {
        auto& states = *hack;

        if (states.event_histories[bucket_index].size() >= 8) {
            states.event_histories[bucket_index].erase(states.event_histories[bucket_index].begin());
        }

        int next_value = !states.event_histories[bucket_index].empty()
                             ? *std::max_element(states.event_histories[bucket_index].begin(), states.event_histories[bucket_index].end()) + 1
                             : states.event_counters[bucket_index];

        states.event_histories[bucket_index].push_back(next_value);
    }

    void CRICMILocalDetector::raiseAlert(int bucket_index) {
        std::cout << "Alert: Cyclic interference detected in Bucket " << bucket_index << "!\n";

        auto& states = *hack;
        const auto& event_history = states.event_histories[bucket_index - 1];
        std::cout << "Event Histories for Bucket " << bucket_index << ": ";
        for (int val : event_history) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    void CRICMILocalDetector::checkInterval() {
        auto& states = *hack;

        states.interval_counter++;
        if (states.interval_counter >= interval_limit) {
            states.interval_counter = 0;
            std::fill(states.event_counters.begin(), states.event_counters.end(), 0);

            for (auto& history : states.event_histories) {
                std::fill(history.begin(), history.end(), 0);
            }
        }
    }

    void CRICMILocalDetector::simulateAccess(int previous_domain, int request_domain, int current_domain) {
        detectCyclicInterference(previous_domain, request_domain, current_domain);
        checkInterval();
    }


} // namespace gem5


