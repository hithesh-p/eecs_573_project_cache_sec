
class CRICMIGlobalDetector:
    def __init__(self, initial_thresholds = [3,3,3,3], decay_rate = 1, increase_rate = 1, frequency_threshold = 4):
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

        if result == 1:
            print(f"                                               Attack at Bucket {bucket_idx}!    Threshold: {self.thresholds}")
        else:
            print(f"                                                      Safe!           Threshold: {self.thresholds}")
        
        return result




detector = CRICMIGlobalDetector(
            initial_thresholds=[3,3,3,3],
            decay_rate=2,
            increase_rate=1,
            frequency_threshold=3
        )

def assertEqual(actual, expected, message=None):
    if actual != expected:
        print(message)

def test_classifyAttack():
    print("             Bucket      Acceess Num   ")
    print("Access 1       0              2")
    result = detector.classifyAttack(0, [7,8,2])
    # print(detector.thresholds)

    print("Access 2       1              2")
    result = detector.classifyAttack(1, [5,6,2])
    # print(detector.thresholds)

    print("Access 3       0              3")
    result = detector.classifyAttack(0, [8,9,3])
    # print(detector.thresholds)

    print("Access 4       0              4")
    result = detector.classifyAttack(0, [5,7,4])
    # print(detector.thresholds)

    print("Access 5       1              3")
    result = detector.classifyAttack(1, [5,6,3])
    # print(detector.thresholds)

    print("Access 6       0              5")
    result = detector.classifyAttack(0, [5,7,5])
    # print(detector.thresholds)

    print("Access 7       2              2")
    result = detector.classifyAttack(2, [5,6,2])
    # print(detector.thresholds)

    print("Access 8       2              3")
    result = detector.classifyAttack(2, [5,6,3])
    # print(detector.thresholds)

    print("Access 9       0              6")
    result = detector.classifyAttack(0, [5,7,6])
    # print(detector.thresholds)

    print("Access 10      0              7")
    result = detector.classifyAttack(0, [5,7,7])
    # print(detector.thresholds)

    print("Access 11      0              8")
    result = detector.classifyAttack(0, [5,7,8])
    # print(detector.thresholds)

    ###



if __name__ == "__main__":
    test_classifyAttack()
