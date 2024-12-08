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

private:
    int threshold;
    int num_buckets;
    int interval_limit;
    int mapper_id;
};

} // namespace gem5

#endif // __CRICMI_LOCAL_DETECTOR_HH__
