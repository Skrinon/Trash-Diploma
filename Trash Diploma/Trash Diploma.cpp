#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <numeric>
#include <sstream>

double calculateMean(const std::vector<double>& values) {
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double calculateStdDev(const std::vector<double>& values, double mean) {
    double sum = 0.0;
    for (double value : values) {
        sum += std::pow(value - mean, 2);
    }
    return std::sqrt(sum / (values.size() - 1));
}

// Updated main function to handle comma-separated values
int main() {
    std::string filename;
    std::cout << "Enter the filename: ";
    std::cin >> filename;

    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 1;
    }

    std::vector<double> data;
    std::string line;
    // Assuming all numbers are on a single line, separated by commas
    if (getline(inputFile, line)) {
        std::stringstream ss(line);
        double temp;
        while (ss >> temp) {
            data.push_back(temp);
            if (ss.peek() == ',') {
                ss.ignore(); // Ignore the comma
            }
            std::cout << temp << std::endl; // Display the number
        }
    }

    inputFile.close();

    if (data.empty()) {
        std::cerr << "The file is empty or contains non-numeric data." << std::endl;
        return 1;
    }

    double mean = calculateMean(data);
    double stdDev = calculateStdDev(data, mean);

    std::cout << "\nMean Value: " << mean << std::endl;
    std::cout << "Standard Deviation: " << stdDev << std::endl;

    return 0;
}