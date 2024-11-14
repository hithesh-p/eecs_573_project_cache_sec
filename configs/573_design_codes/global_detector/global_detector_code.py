import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
from sklearn.ensemble import RandomForestClassifier
import joblib  # Import joblib for saving and loading the model
import os

class CRICMIGlobalDetector:
    def __init__(self, num_buckets=4, set_classifier=RandomForestClassifier(n_estimators=100, random_state=42)):
        self.num_buckets = num_buckets
        self.model_file = "global_detector_model.pkl"  # File to save the model
        
        # Check if the model file exists and load it, otherwise train a new model
        if os.path.exists(self.model_file):
            self.classifier = self.load_model()
            print("Model loaded from file.")
        else:
            self.classifier = self.train_Classifier(set_classifier)
            self.save_model()
            print("Model trained and saved to file.")

    def attackPrediction(self, event_histories):
        sample = self.feature_Extraction(event_histories)
        res = self.classifier.predict([sample])
        if res[0] == 1:
            print("1 ")  # Cache Attack Detected
        else:
            print("0 ")  # No Attack

    def feature_Extraction(self, event_histories):
        max_alerts = max(event_histories)
        mean_alerts = sum(event_histories) / len(event_histories)
        return [max_alerts, mean_alerts]

    def train_Classifier(self, set_classifier):
        dataset = pd.read_csv("dataset.csv")
        features = dataset[['max_alerts', 'mean_alerts']].values
        labels = dataset['label'].values

        X_train, X_test, y_train, y_test = train_test_split(features, labels, test_size=0.3, random_state=42)
        classifier = set_classifier
        classifier.fit(X_train, y_train)

        y_pred = classifier.predict(X_test)
        accuracy = accuracy_score(y_test, y_pred)
        print(f"Accuracy: {accuracy}")

        return classifier

    def save_model(self):
        joblib.dump(self.classifier, self.model_file)
        print(f"Model saved to {self.model_file}.")

    def load_model(self):
        return joblib.load(self.model_file)

