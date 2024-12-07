#include "cricmi_global_detector.hh"
#include "base/debug.hh"

namespace gem5 {

CRICMIGlobalDetector::CRICMIGlobalDetector(const CRICMIGlobalDetectorParams &params)
    : SimObject(params),
      decay_rate(params.decay_rate),
      increase_rate(params.increase_rate),
      frequency_threshold(params.frequency_threshold),
      thresholds(params.thresholds),
      bucket_frequencies(params.bucket_frequencies),
      last_occurrence(params.last_occurrence),
      mapper_id(params.mapper_id) {}



int CRICMIGlobalDetector::classifyAttack(int history_last) {
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

    return result;
}

} // namespace gem5