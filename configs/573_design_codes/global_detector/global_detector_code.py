from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, confusion_matrix
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC
from sklearn.ensemble import RandomForestClassifier


class CRICMIGlobalDetector:
    def __init__(self, num_buckets=4, set_classifier = RandomForestClassifier(n_estimators=100, random_state=42)):
        # TODO :: GD constructor

        #no of buckets for holding the event history
        self.num_buckets = num_buckets
        self.classifier = self.train_Classifier(set_classifier)
    
    def attackPrediction(self, event_histories):
        # TODO :: Need some way to receive event_histories from local detectors
        sample = self.feature_Extraction(event_histories)

        res = self.classifier.predict([sample])
        if res[0] == 1:
            # print("Cache Attack Detected!")
            print("1 ")
        else:
            # print("No Attack!")
            print("0 ")

    def feature_Extraction(self, event_histories):
        # Calculate the maximum
        max_alerts = max(event_histories)

        # Calculate the mean
        mean_alerts = sum(event_histories) / len(event_histories)

        return [max_alerts, mean_alerts]

    # TODO :: Need to use better data to train the classifier
    def train_Classifier(self, set_classifier):
        # Example feature data and labels
        features = [
            [5, 2.5], [7, 3.2], [12, 6.1], [9, 4.8], [3, 1.4],  # Example benign features
            [15, 7.2], [14, 6.8], [18, 9.5], [13, 5.9], [16, 8.0]  # Example attack features
        ]
        labels = [0, 0, 1, 1, 0, 1, 1, 1, 1, 1]  # 0 = benign, 1 = malicious

        # Split the data
        X_train, X_test, y_train, y_test = train_test_split(features, labels, test_size=0.3, random_state=42)

        # Choose and train the classifier
        classifier = set_classifier
        classifier.fit(X_train, y_train)


        # Make predictions and evaluate
        y_pred = classifier.predict(X_test)
        accuracy = accuracy_score(y_test, y_pred)


        print(f"Accuracy: {accuracy}")

        return classifier
