class CRICMILocalDetector:
    def __init__(self, threshold=8, num_buckets=4, interval_limit=32):
        #threshold used fo raising alerts
        self.threshold = threshold

        #no of buckets for holding the event history
        self.num_buckets = num_buckets

        #event counter for triggering
        self.event_counter = 0 # 8 bit counter wraps at 256
        # event history registers (each are 8 bit) per bucket
        self.event_histories = [[0]*8 for _ in range(num_buckets)]

        #we use the interval counter to wrap and or reset after a specific limit
        self.interval_counter = 0
        self.interval_limit = interval_limit

    def detect_cyclic_interference(self, previous_domain, request_domain, current_domain):
        #if previous_domain != current_domain and current_domain == request_domain:
        #for the a->b->a cyclic pattern detection
        if previous_domain==request_domain and request_domain!=current_domain:
            self.event_counter = (self.event_counter + 1) % 256 #cyclic inteference is detected so increase event countwr for the bucket

            #check for threshold hit
            if self.event_counter >= self.threshold:
                self.raise_alert()
                self.event_counter = 0 #setting back the counter to 0 after sending alert

            #to shift the event histories (older ones get pushed to make room for newer one)
            self.update_event_histories()

    def update_event_histories(self):
        for i in range(self.num_buckets):
            self.event_histories[i].append(self.event_counter)
            self.event_histories[i].pop(0) #delete the last entry

    def raise_alert(self):
        print("Alert: Cyclic interference detected!")
        #more parts to add so we can send a signal to the global detector

    def check_interval(self):
        self.interval_counter += 1
        if self.interval_counter >= self.interval_limit:
            self.interval_counter =0
            self.event_counter = 0

    def simulate_access(self, previous_domain, request_domain, current_domain):
        self.detect_cyclic_interference(previous_domain, request_domain, current_domain)
        self.check_interval()


#ld = CRICMILocalDetector(threshold=8, num_buckets=4, interval_limit=32)
