#ifndef __CRICMI_GLOBAL_DETECTOR_HH__
#define __CRICMI_GLOBAL_DETECTOR_HH__

#include "params/CRICMIGlobalDetector.hh" // Auto-generated params

#include "sim/sim_object.hh"
#include "mem/port.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"

namespace gem5{

class CRICMIGlobalDetector : public SimObject {
    public:
        struct BucketInfo {
            uint8_t bucket_idx;
            uint32_t event_history;
            int detector_type;

        } bucket_info;

        // Port definition
        class CRICMIMemSidePort : public ResponsePort {
            public:
                CRICMIMemSidePort(const std::string &name, CRICMIGlobalDetector *detector);
                bool recvTimingReq(PacketPtr pkt);

                // Implement required pure virtual methods
                gem5::Tick recvAtomic(gem5::PacketPtr pkt) override {
                    // Default implementation (no action)
                    return 0;
                }

                void recvRespRetry() override {
                    // Default implementation (no action)
                }

                void recvFunctional(gem5::PacketPtr pkt) override {
                    // Default implementation (no action)
                }

                gem5::AddrRangeList getAddrRanges() const override {
                    // Return an empty address range as default
                    return gem5::AddrRangeList();
                }

            private:
                CRICMIGlobalDetector *detector; // Response Port need the class object that use it
        };

        CRICMIMemSidePort memSidePort;

        CRICMIGlobalDetector(const CRICMIGlobalDetectorParams &params);
        
    
    private:
        std::vector<int> thresholds;
        int decay_rate;
        int increase_rate;
        int frequency_threshold;
        std::vector<int> bucket_frequencies;
        std::vector<int> last_occurrence;

        int bucket_idx = 0;
        // uint32_t event_history; // Stores recent event counts
        int detector_type;

        int mapper_id;

        int classifyAttack(uint32_t event_history); // No parameter needed; uses `eventHistory` directly
    

};

}

#endif