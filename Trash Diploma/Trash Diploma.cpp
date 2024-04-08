#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <numeric>
#include <sstream>
#include <map>
#include <algorithm>

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

std::map<int, double> readQuantiles(const std::string& filename) {
    std::map<int, double> quantiles;
    std::ifstream file(filename);
    std::string line;

    while (getline(file, line)) {
        std::stringstream ss(line);
        int n;
        double t;
        if (ss >> n >> t) {
            quantiles[n] = t;
        }
    }
    return quantiles;
}

double calculateConfidenceInterval(double stdDev, double quantile, int sampleSize) {
    return quantile * stdDev / std::sqrt(sampleSize);
}

std::vector<double> readAndConvertData(const std::string& filename) {
    std::vector<double> data;
    std::ifstream inputFile(filename);
    std::string line, value;

    while (getline(inputFile, line)) {
        std::replace(line.begin(), line.end(), ',', '.'); // Замена запятых на точки
        std::stringstream ss(line);
        while (getline(ss, value, '.')) { // Разделение по точке, предполагая, что запятые уже заменены
            data.push_back(std::stod(value));
        }
    }

    return data;
}

int main() {
    std::locale::global(std::locale(""));
    std::string dataFilename, quantilesFilename;
    std::cout << "Введите имя файла с данными: ";
    std::cin >> dataFilename;
    std::cout << "Введите имя файла с квантилями: ";
    std::cin >> quantilesFilename;

    auto data = readAndConvertData(dataFilename);
    auto quantiles = readQuantiles(quantilesFilename);

    if (data.empty()) {
        std::cerr << "Файл с данными пуст или содержит нечисловые данные." << std::endl;
        return 1;
    }

    double mean = calculateMean(data);
    double stdDev = calculateStdDev(data, mean);
    int sampleSize = data.size();

    double quantile = quantiles.count(sampleSize) ? quantiles[sampleSize] : quantiles.rbegin()->second;
    double confidenceInterval = calculateConfidenceInterval(stdDev, quantile, sampleSize);

    std::cout << "\nСреднее значение: " << mean << std::endl;
    std::cout << "Среднеквадратичное отклонение: " << stdDev << std::endl;
    std::cout << "95% доверительный интервал: " << confidenceInterval << std::endl;

    return 0;
}
