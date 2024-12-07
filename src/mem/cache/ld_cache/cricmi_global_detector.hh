#ifndef __CRICMI_GLOBAL_DETECTOR_HH__
#define __CRICMI_GLOBAL_DETECTOR_HH__

#include "params/CRICMIGlobalDetector.hh" // Auto-generated params
#include "sim/sim_object.hh"

#include "mem/port.hh"
#include "mem/packet.hh"

namespace gem5{

class CRICMIGlobalDetector : public SimObject {
    public:
        CRICMIGlobalDetector(const CRICMIGlobalDetectorParams &params);

        int classifyAttack(int history_last); // No parameter needed; uses `eventHistory` directly
    private:
        std::vector<int> thresholds;
        int decay_rate;
        int increase_rate;
        int frequency_threshold;
        std::vector<int> bucket_frequencies;
        std::vector<int> last_occurrence;

        int bucket_idx = 0;
        std::vector<int> event_History; // Stores recent event counts

        int mapper_id;
    

};

}

#endif