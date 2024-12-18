#ifndef __CRICMI_GLOBAL_DETECTOR_HH__
#define __CRICMI_GLOBAL_DETECTOR_HH__

#include "params/CRICMIGlobalDetector.hh" // Auto-generated params
#include "params/CRICMILocalDetector.hh" // Auto-generated params

#include "sim/sim_object.hh"
#include "mem/port.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"

namespace gem5{

class CRICMIGlobalDetector : public SimObject {
    friend class CRICMILocalDetector;

    public:

        // Port definition
        class CRICMICPUSidePort : public ResponsePort {
            public:
                CRICMICPUSidePort(const std::string &name, CRICMIGlobalDetector *detector);
                bool recvTimingReq(PacketPtr pkt) override;

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
                CRICMIGlobalDetector *detector;
        };

        CRICMICPUSidePort cpu_side_port;

        Port &getPort(const std::string &if_name,
                      PortID idx=InvalidPortID) override;


        CRICMIGlobalDetector(const CRICMIGlobalDetectorParams &params);
        int classifyAttack(int bucket_idx, int history_last); // No parameter needed; uses `eventHistory` directly
    
    private:
        std::vector<int> thresholds;
        int decay_rate;
        int increase_rate;
        int frequency_threshold;
        std::vector<int> bucket_frequencies;
        std::vector<int> last_occurrence;

        // int bucket_idx = 0;
        std::vector<int> event_History; // Stores recent event counts

        int mapper_id;
    
        static CRICMIGlobalDetector *hack_global_detector;

};

}

#endif
