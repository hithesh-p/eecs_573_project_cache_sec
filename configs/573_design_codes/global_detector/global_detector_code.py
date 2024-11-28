
class CRICMIGlobalDetector:
    def __init__(self, initial_thresholds = [10,10,10,10], decay_rate = 1, increase_rate = 1, frequency_threshold = 5):
        self.thresholds = initial_thresholds  # Initial thresholds for 4 buckets
        self.decay_rate = decay_rate          # Rate of decreasing threshold
        self.increase_rate = increase_rate    # Rate of increasing threshold
        self.frequency_threshold = frequency_threshold
        self.bucket_frequencies = [0] * len(initial_thresholds)
        self.last_occurrence = [0] * len(initial_thresholds)

    def classifyAttack(self, bucket_idx, event_history):
        result = 0
        
        for i in range(len(self.bucket_frequencies)):
            if i == bucket_idx:
                if event_history[-1] > self.thresholds[bucket_idx]:
                    result = 1
                self.bucket_frequencies[bucket_idx] += 1
                self.last_occurrence[bucket_idx] = 0
            else:
                self.last_occurrence[i] += 1
        
        #Update the thredsholds
        for i in range(len(self.thresholds)):
            if self.bucket_frequencies[i] >= self.frequency_threshold:
                if self.thresholds[i] >= self.decay_rate:
                    self.thresholds[i] -= self.decay_rate  # Reduce threshold
                self.bucket_frequencies[i] = 0
            if self.last_occurrence[i] >= self.frequency_threshold:
                self.thresholds[i] += self.increase_rate  # Increase threshold
                self.last_occurrence[i] = 0

        return result