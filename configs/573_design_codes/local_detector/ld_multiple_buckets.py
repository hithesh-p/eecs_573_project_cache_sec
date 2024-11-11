import random


class CRICMILocalDetector:
    def __init__(self, threshold=8, num_buckets=4, interval_limit=32):
        #threshold used fo raising alerts
        self.threshold = threshold

        #no of buckets for holding the event history
        self.num_buckets = num_buckets

        #event counter for triggering
        self.event_counters = [0] * num_buckets # 8 bit counter for each bucket
        # event history registers (each are 8 bit) per bucket
        self.event_histories = [[0]*8 for _ in range(num_buckets)]

        #we use the interval counter to wrap and or reset after a specific limit
        self.interval_counter = 0
        self.interval_limit = interval_limit

    def detect_cyclic_interference(self, previous_domain, request_domain, current_domain):
        #if previous_domain != current_domain and current_domain == request_domain:
        for i in range(self.num_buckets):
            #for the a->b->a cyclic pattern detection
            if previous_domain==i+1 and request_domain == i+1 and request_domain!=current_domain:
                self.event_counters[i] = (self.event_counters[i] + 1) % 256 #cyclic inteference is detected so increase event countwr for the bucket

                #check for threshold hit
                if self.event_counters[i] >= self.threshold:
                    self.raise_alert(i)
                    self.event_counters[i] = 0 #setting back the counter to 0 after sending alert

                #to shift the event histories (older ones get pushed to make room for newer one)
                self.update_event_histories(i)

    def update_event_histories(self, bucket_index):
        self.event_histories[bucket_index].append(self.event_counters[bucket_index])
        if len(self.event_histories[bucket_index]) > 8:
            self.event_histories[bucket_index].pop(0) #delete the last entry

    def raise_alert(self, bucket_index):
        print(f"Alert: Cyclic interference detected in Bucket {bucket_index}!")
        #more parts to add so we can send a signal to the global detector

    def check_interval(self):
        self.interval_counter += 1
        if self.interval_counter >= self.interval_limit:
            self.interval_counter =0
            self.event_counters = [0] * self.num_buckets

    def simulate_access(self, previous_domain, request_domain, current_domain):
        self.detect_cyclic_interference(previous_domain, request_domain, current_domain)
        self.check_interval()


#ld = CRICMILocalDetector(threshold=8, num_buckets=4, interval_limit=32)

# testing
def test_CRICMILocalDetector():

    #diff buckets for diff domains-> bucket 0 for domain 1, bucket 1 for domain 2 ...
    detector = CRICMILocalDetector(threshold=3, num_buckets=4, interval_limit=10)

    # list of access patterns -> tuples
    access_patterns = [
        (1, 1, 2),
        (2, 2, 1),
        (3, 3, 1),
        (4, 4, 3),
        (1, 1, 3),
        (2, 2, 3),
        (3, 3, 2),
        (2, 4, 1),
        (1, 1, 2),
        (2, 2, 1),
        (3, 3, 1),
        (4, 4, 3),
    ]

    # debugging printing
    for i, (previous_domain, request_domain, current_domain) in enumerate(access_patterns):
        print(f"\nAccess {i + 1}: Previous Domain={previous_domain}, Request Domain={request_domain}, Current Domain={current_domain}")
        detector.simulate_access(previous_domain, request_domain, current_domain)

        # output for the current state of event counter and history
        print(f"Event Counter (8-bit per bucket): {detector.event_counters}")
        print(f"Event Histories: {detector.event_histories}")
        print(f"Interval Counter: {detector.interval_counter}")


test_CRICMILocalDetector()
