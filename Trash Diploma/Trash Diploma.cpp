﻿#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <numeric>
#include <sstream>
#include <string>

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

int main() {
    std::locale::global(std::locale(""));
    std::string filename;
    std::cout << "Введите имя файла: ";
    std::cin >> filename;

    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return 1;
    }

    std::vector<double> data;
    std::string line;
    if (getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string value;
        while (getline(ss, value, ',')) { // Используем getline для разделения по запятой
            data.push_back(std::stod(value)); // Преобразуем строку в число
            std::cout << std::stod(value) << std::endl; // Отображаем число
        }
    }

    inputFile.close();

    if (data.empty()) {
        std::cerr << "Файл пуст или содержит нечисловые данные." << std::endl;
        return 1;
    }

    double mean = calculateMean(data);
    double stdDev = calculateStdDev(data, mean);

    std::cout << "\nСреднее значение: " << mean << std::endl;
    std::cout << "Среднеквадратичное отклонение: " << stdDev << std::endl;

    return 0;
}
