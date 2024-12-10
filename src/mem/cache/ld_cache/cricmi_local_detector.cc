#include "cricmi_local_detector.hh"
#include "cricmi_global_detector.hh"

#include <cstdio>
#include <iostream>

namespace gem5 {

int CRICMILocalDetector::global_mapper_count = 0;

CRICMILocalDetector::CRICMILocalDetector(const CRICMILocalDetectorParams &params)
    : SimObject(params),
      mem_side_port(params.name + ".mem_side_port", this),
      threshold(params.threshold),
      num_buckets(params.num_buckets),
      interval_limit(params.interval_limit),
      mapper_id(global_mapper_count++)
      // mapper_id(params.mapper_id)
{
    // not needed 
    // Initialize internal data structures or logging if needed
    // event_counters.resize(num_buckets, 0);
    // event_histories.resize(num_buckets, std::vector<int>(8, 0));
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


void CRICMILocalDetector::detectCyclicInterference(Addr addr, int previous_domain, int request_domain, int current_domain) {
    event_data_map.emplace(addr, EventData(num_buckets));
    auto &states = get_event_state(addr);

    // printf("previous_domain %d request_domain %d current_domain %d\n",
            // previous_domain, request_domain, current_domain);

    for (int i = 1; i <= num_buckets; ++i) {

        if (previous_domain == i && request_domain == i && request_domain != current_domain) {
            states.event_counters[i - 1] = (states.event_counters[i - 1] + 1) % 256;
            printf("alert raised i=%d num_buckets=%d, states.event_counters[i - 1]=%d\n",
                    i, num_buckets, states.event_counters[i - 1]);

            if (states.event_counters[i - 1] >= threshold) {
                raiseAlert(addr, i);
                states.event_counters[i - 1] = 0;
            }

            updateEventHistories(addr, i - 1);
        }
    }
}

void CRICMILocalDetector::updateEventHistories(Addr addr, int bucket_index) {
    event_data_map.emplace(addr, EventData(num_buckets));
    auto &states = get_event_state(addr);

    if (states.event_histories[bucket_index].size() >= 8) {
        states.event_histories[bucket_index].erase(states.event_histories[bucket_index].begin());
    }

    int next_value = !states.event_histories[bucket_index].empty()
                            ? *std::max_element(states.event_histories[bucket_index].begin(), states.event_histories[bucket_index].end()) + 1
                            : states.event_counters[bucket_index];

    states.event_histories[bucket_index].push_back(next_value);
}

void CRICMILocalDetector::raiseAlert(Addr addr, int bucket_index) {
    std::cout << "Alert: Cyclic interference detected in Bucket " << bucket_index << "!\n";

    auto &states = get_event_state(addr);
    const auto& event_history = states.event_histories[bucket_index - 1];
    std::cout << "Event Histories for Bucket " << bucket_index << ": ";
    for (int val : event_history) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    CRICMIGlobalDetector::hack_global_detector->classifyAttack(bucket_index, event_history[event_history.size() - 1]);

}

void CRICMILocalDetector::checkInterval(Addr addr) {
    auto &states = get_event_state(addr);

    states.interval_counter++;
    if (states.interval_counter >= interval_limit) {
        states.interval_counter = 0;
        std::fill(states.event_counters.begin(), states.event_counters.end(), 0);

        for (auto& history : states.event_histories) {
            std::fill(history.begin(), history.end(), 0);
        }
    }
}

void CRICMILocalDetector::simulateAccess(Addr addr, int request_domain) {
    // addr = addr & 0x3ffc0;
    addr = addr & 0xfc0;

    // debug: only see CPU 2
    // if (request_domain != 10)
        // return;

    // EECS573 hack: only see two CPUs
    if (request_domain != 6 && request_domain != 10)
        return;

    if (0 || (addr >> 6) % 64 == 54) {
        printf("mapper_id %d: addr %llx domain %d\n",
                mapper_id, addr >> 6, request_domain);
    }

    // to retain previous state values
    // getting the previous and current domains for the given address
    int &previous_domain = previous_domain_map[addr];
    int &current_domain = current_domain_map[addr];

    if (request_domain == request_domain_map[addr])
        return;

    request_domain_map[addr] = request_domain;

    if (0 || (addr >> 6) % 64 == 54) {
        printf("mapper_id %d: previous_domain %d current_domain %d request_domain %d\n",
                mapper_id, previous_domain, current_domain, request_domain);
        fflush(stdout);
    }

    //call with the updated new values
    detectCyclicInterference(addr, previous_domain, request_domain, current_domain);

    previous_domain = current_domain;
    current_domain = request_domain;
    checkInterval(addr);
}

CRICMILocalDetector::EventData &CRICMILocalDetector::get_event_state(Addr addr) {
    auto it = event_data_map.find(addr);
    if (it == event_data_map.end())
        fatal("EECS573: Cannot find map for addr %d\n", addr);
    return it->second;
}

} // namespace gem5


