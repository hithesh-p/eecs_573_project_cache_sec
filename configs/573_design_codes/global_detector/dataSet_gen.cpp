#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

// Function to generate a random integer between min and max
int generateRandomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Function to generate a random double between min and max
double generateRandomDouble(double min, double max) {
    return min + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (max - min)));
}

// Main function to generate the dataset
int main() {
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    // Number of samples to generate
    int num_samples = 10000;
    
    // Parameters for benign and attack data ranges
    int benign_max_min = 1, benign_max_max = 10;        // Positive integer range for benign max
    double benign_mean_min = 0.1, benign_mean_max = 5.0; // Positive double range for benign mean
    int attack_max_min = 11, attack_max_max = 20;       // Positive integer range for attack max
    double attack_mean_min = 5.1, attack_mean_max = 10.0; // Positive double range for attack mean

    // Open a CSV file to write the dataset
    std::ofstream file("dataset.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open the file for writing." << std::endl;
        return 1;
    }

    // Write the header
    file << "max_alerts,mean_alerts,label\n";

    // Generate benign samples
    for (int i = 0; i < num_samples / 2; ++i) {
        int max_alerts = generateRandomInt(benign_max_min, benign_max_max);
        double mean_alerts = generateRandomDouble(benign_mean_min, benign_mean_max);
        file << max_alerts << "," << mean_alerts << ",0\n"; // Label 0 for benign
    }

    // Generate attack samples
    for (int i = 0; i < num_samples / 2; ++i) {
        int max_alerts = generateRandomInt(attack_max_min, attack_max_max);
        double mean_alerts = generateRandomDouble(attack_mean_min, attack_mean_max);
        file << max_alerts << "," << mean_alerts << ",1\n"; // Label 1 for attack
    }

    // Close the file
    file.close();
    std::cout << "Dataset generated successfully and saved to dataset.csv" << std::endl;

    return 0;
}
