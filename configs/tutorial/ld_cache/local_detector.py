import random
from m5.objects import SimObject
from m5.params import Param, VectorParam, Int

class CRICMILocalDetector(SimObject):
    type = 'CRICMILocalDetector'
    cxx_header = "/configs/tutorial/ld_cache/cricmi_local_detector.cc"
    cxx_class = "gem5::CRICMILocalDetector"

    # Declare parameters
    threshold = Param.Int(8, "Threshold for raising alerts")
    num_buckets = Param.Int(4, "Number of buckets for holding event history")
    interval_limit = Param.Int(32, "Interval counter reset limit")
    # TODO: Ruiying needs this so it can be passed to the global detector
    # alert_callback = Param.SimObject(None)

    
    
    
    # Initialization happens post-instantiation in gem5
    def startup(self):
        # Internal data structures
        self.event_counters = [0] * self.num_buckets  # 8-bit counter for each bucket
        self.event_histories = [[0] * 8 for _ in range(self.num_buckets)]
        self.interval_counter = 0


    def detect_cyclic_interference(self, previous_domain, request_domain, current_domain):
        #if previous_domain != current_domain and current_domain == request_domain:
        for i in range(1, self.num_buckets+1):
            #for the a->b->a cyclic pattern detection
            if previous_domain==i and request_domain == i and request_domain!=current_domain:
                self.event_counters[i-1] = (self.event_counters[i-1] + 1) % 256 #cyclic inteference is detected so increase event countwr for the bucket

                #check for threshold hit
                if self.event_counters[i-1] >= self.threshold:
                    self.raise_alert(i)
                    self.event_counters[i-1] = 0 #setting back the counter to 0 after sending alert

                #to shift the event histories (older ones get pushed to make room for newer one)
                self.update_event_histories(i-1)

    def update_event_histories(self, bucket_index):
        # self.event_histories[bucket_index].append(self.event_counters[bucket_index])
        # if len(self.event_histories[bucket_index]) >= 8:
        #     self.event_histories[bucket_index].pop(0) #delete the last entry
        # Shift history for each bucket and add the current event count
        if len(self.event_histories[bucket_index]) >= 8:
            self.event_histories[bucket_index].pop(0)   # Maintain only recent history (max of 8 entries)

        # Append new value sequentially without resetting to zero
        next_value = max(self.event_histories[bucket_index]) + 1 if max(self.event_histories[bucket_index]) > 0 else self.event_counters[bucket_index]
        self.event_histories[bucket_index].append(next_value)

    # here is where we use the functionality to send the output to GD
    def raise_alert(self, bucket_index):
        print(f"Alert: Cyclic interference detected in Bucket {bucket_index}!")
        print(f"Event Histories for Bucket {bucket_index}: {self.event_histories[bucket_index-1]}")
        #more parts to add so we can send a signal to the global detector
        #commented this- remeber to tell Ruiying
        # if self.alert_callback:
        #     self.alert_callback(self.event_histories[bucket_index-1], bucket_index, alert=True)

        # TODO: Ruiying needs this so it can be passed to the global detector
        # Handle callback
        # if self.alert_callback:
        #     self.alert_callback.alert(bucket_index=bucket_index, history=self.event_histories[bucket_index - 1])
        # else:
        #     print("No alert callback connected.")



    def check_interval(self):
        self.interval_counter += 1
        if self.interval_counter >= self.interval_limit:
            self.interval_counter =0
            self.event_counters = [0] * self.num_buckets

            # Reset all event histories to zero when interval resets.
            for i in range(self.num_buckets):
                self.event_histories[i] = [0] * len(self.event_histories[i])

    def simulate_access(self, previous_domain, request_domain, current_domain):
        self.detect_cyclic_interference(previous_domain, request_domain, current_domain)
        self.check_interval()


#ld = CRICMILocalDetector(threshold=8, num_buckets=4, interval_limit=32)

# testing; might not be needed to initialize here
# def test_CRICMILocalDetector():

#     #just a dummy return from the callback
#     # no need alert
#     def global_detector_callback(event_history, bucket_index, alert):
#         print(f"Global Detector Callback Received from Bucket {bucket_index}: ", event_history, "Alert:", alert)

#     #diff buckets for diff domains-> bucket 0 for domain 1, bucket 1 for domain 2 ...
#     detector = CRICMILocalDetector(threshold=3, num_buckets=4, interval_limit=10, alert_callback=global_detector_callback)

#     # list of access patterns -> tuples
#     access_patterns = [
#         (1, 1, 2),
#         (2, 2, 1),
#         (3, 3, 1),
#         (4, 4, 3),
#         (1, 1, 3),
#         (2, 2, 3),
#         (3, 3, 2),
#         (2, 4, 1),
#         (1, 1, 2),
#         (2, 2, 1),
#         (3, 3, 1),
#         (4, 4, 3),
#     ]

#     # debugging printing
#     for i, (previous_domain, request_domain, current_domain) in enumerate(access_patterns):
#         print(f"\nAccess {i + 1}: Previous Domain={previous_domain}, Request Domain={request_domain}, Current Domain={current_domain}")
#         detector.simulate_access(previous_domain, request_domain, current_domain)

#         # output for the current state of event counter and history
#         print(f"Event Counter (8-bit per bucket): {detector.event_counters}")
#         print(f"Event Histories: {detector.event_histories}")
#         print(f"Interval Counter: {detector.interval_counter}")

#         #printing the alert signal passed and the event histories passed
#         for bucket_index in range(1, detector.num_buckets+1):
#             # if detector.event_counters[bucket_index] >= detector.threshold:
#             print(f"Bucket {bucket_index} Event Histories: {detector.event_histories[bucket_index-1]}")

# test_CRICMILocalDetector()


