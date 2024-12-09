#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

#include <algorithm>

class CRICMILocalDetectorHack {
public:
    std::vector<int> event_counters;
    std::vector<std::vector<int>> event_histories;
    int interval_counter = 0;
};

//global variable
std::mutex global_lock;
int global_data_mapper_id = 0;
std::map<int, std::shared_ptr<CRICMILocalDetectorHack>> global_data_mapper;

class CRICMILocalDetector {
public:
    int threshold;
    int num_buckets;
    int interval_limit;
    int mapper_id;
    std::shared_ptr<CRICMILocalDetectorHack> hack;

    CRICMILocalDetector(int threshold, int num_buckets, int interval_limit)
        : threshold(threshold), num_buckets(num_buckets), interval_limit(interval_limit) {}

    void startup() {
        hack = std::make_shared<CRICMILocalDetectorHack>();
        hack->event_counters.resize(num_buckets, 0);
        hack->event_histories.resize(num_buckets, std::vector<int>(8, 0));

        // added locks as per the suggestion
        std::lock_guard<std::mutex> lock(global_lock);
        global_data_mapper_id++;
        mapper_id = global_data_mapper_id;
        global_data_mapper[mapper_id] = hack;
    }

    void detectCyclicInterference(int previous_domain, int request_domain, int current_domain) {
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

    void updateEventHistories(int bucket_index) {
        auto& states = *hack;

        if (states.event_histories[bucket_index].size() >= 8) {
            states.event_histories[bucket_index].erase(states.event_histories[bucket_index].begin());
        }

        int next_value = !states.event_histories[bucket_index].empty()
                             ? *std::max_element(states.event_histories[bucket_index].begin(), states.event_histories[bucket_index].end()) + 1
                             : states.event_counters[bucket_index];

        states.event_histories[bucket_index].push_back(next_value);
    }

    void raiseAlert(int bucket_index) {
        std::cout << "Alert: Cyclic interference detected in Bucket " << bucket_index << "!\n";

        auto& states = *hack;
        const auto& event_history = states.event_histories[bucket_index - 1];
        std::cout << "Event Histories for Bucket " << bucket_index << ": ";
        for (int val : event_history) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    void checkInterval() {
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

    void simulateAccess(int previous_domain, int request_domain, int current_domain) {
        detectCyclicInterference(previous_domain, request_domain, current_domain);
        checkInterval();
    }
};

// test function
void test_CRICMILocalDetector() {
    CRICMILocalDetector detector(3, 4, 10);
    detector.startup();

    std::vector<std::tuple<int, int, int>> access_patterns = {
        {1, 1, 2}, {2, 2, 1}, {3, 3, 1}, {4, 4, 3},
        {1, 1, 3}, {2, 2, 3}, {3, 3, 2}, {2, 4, 1},
        {1, 1, 2}, {2, 2, 1}, {3, 3, 1}, {4, 4, 3},
    };

    for (size_t i = 0; i < access_patterns.size(); ++i) {
        auto [previous, request, current] = access_patterns[i];
        std::cout << "\nAccess " << i + 1
                  << ": Previous Domain=" << previous
                  << ", Request Domain=" << request
                  << ", Current Domain=" << current << std::endl;

        detector.simulateAccess(previous, request, current);
    }
}

int main() {
    test_CRICMILocalDetector();
    return 0;
}


// to compile
// g++ local_detector.cc -o local_detector -std=c++11 -pthread

//to run
// ./local_detector