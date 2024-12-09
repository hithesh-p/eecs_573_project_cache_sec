#ifndef __CRICMI_LOCAL_DETECTOR_HH__
#define __CRICMI_LOCAL_DETECTOR_HH__

#include "params/CRICMILocalDetector.hh" // Auto-generated params
#include "sim/sim_object.hh"
#include "mem/request.hh"
#include "mem/packet.hh"
#include "mem/port.hh"
#include "mem/packet_access.hh"

#include <iostream>

namespace gem5 {

class CRICMILocalDetector : public SimObject {
public:

    // struct to hold the data for each address
    struct EventData {
        std::vector<int> event_counters;
        std::vector<std::vector<int>> event_histories;
        int interval_counter = 0;

        EventData(int num_buckets)
            : event_counters(num_buckets, 0),
              event_histories(num_buckets, std::vector<int>(8, 0)) {}
    };
    
    // Port definition
    class CRICMIMemSidePort : public RequestPort {
        public:
            CRICMIMemSidePort(const std::string &name, CRICMILocalDetector *detector);
            // Implementation of recvTimingResp
            bool recvTimingResp(PacketPtr pkt) override {
                std::cout << "CRICMIMemSidePort: Received timing response: " 
                        << pkt->print() << std::endl;
                // Add custom logic for handling the response here
                return true; // Indicate success
            }

            // Implementation of recvReqRetry
            void recvReqRetry() override {
                std::cout << "CRICMIMemSidePort: Retry event received." << std::endl;
                // Add custom retry logic here
            }


        private:
            CRICMILocalDetector *detector;
    };

    CRICMIMemSidePort mem_side_port;

    CRICMILocalDetector(const CRICMILocalDetectorParams &params);

    Port &getPort(const std::string &if_name,
                  PortID idx=InvalidPortID) override;

    // Method to send data
    void sendData(uint8_t *data);

    void detectCyclicInterference(Addr addr, int previous_domain, int request_domain, int current_domain);
    void updateEventHistories(Addr addr, int bucket_index);
    void raiseAlert(Addr addr, int bucket_index);
    void checkInterval(Addr addr);
    void simulateAccess(Addr addr, int request_domain);


private:
    int threshold;
    int num_buckets;
    int interval_limit;
    int mapper_id;

    std::map<Addr, EventData> event_data_map; // map with address as key and EventData as value
};

} // namespace gem5

#endif // __CRICMI_LOCAL_DETECTOR_HH__
