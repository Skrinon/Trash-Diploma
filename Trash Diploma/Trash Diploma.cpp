#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <numeric>
#include <sstream>
#include <map>
#include <algorithm>

// Функция для вычисления среднего значения массива
double calculateMean(const std::vector<double>& values) {
    if (values.empty()) return std::nan("1");
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

// Функция для вычисления стандартного отклонения
double calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.size() < 2) return std::nan("1");
    double sum = 0.0;
    for (double value : values) {
        sum += std::pow(value - mean, 2);
    }
    return std::sqrt(sum / (values.size() - 1));
}

// Функция для чтения квантилей из файла
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

// Функция для вычисления доверительного интервала
double calculateConfidenceInterval(double stdDev, double quantile, int sampleSize) {
    return quantile * stdDev / std::sqrt(sampleSize);
}

// Функция для чтения данных из файла и их конвертации
std::vector<double> readAndConvertData(const std::string& filename) {
    std::vector<double> data;
    std::ifstream inputFile(filename);
    std::string line, value;
    while (getline(inputFile, line)) {
        std::stringstream ss(line);
        while (getline(ss, value, ',')) {
            data.push_back(std::stod(value));
        }
    }
    return data;
}

// Функции для расчета неопределенностей
double calculateTypeAUncertainty(const std::vector<double>& values, double mean) {
    double sum = 0.0;
    for (double value : values) {
        sum += std::pow(value - mean, 2);
    }
    return std::sqrt(sum / (values.size() * (values.size() - 1)));
}

double calculateTypeBUncertainty(double a_minus, double a_plus, char distributionType) {
    double u_B = 0.0;
    double beta;
    double lambda;
    double x;

    switch (distributionType) {
    case 'U':
        u_B = (a_plus - a_minus) / std::sqrt(12);
        break;
    case 'T':
        u_B = (a_plus - a_minus) / std::sqrt(24);
        break;
    case 'R':
        beta = 0.5;
        u_B = (a_plus - a_minus) / (std::sqrt(24) * std::sqrt(1 + beta * beta));
        break;
    case 'E':
        lambda = 1.0;
        x = (a_plus + a_minus) / 2;
        u_B = std::sqrt(((a_plus - x) * (x - a_minus) - ((a_plus - 2 * x + a_minus) / lambda)));
        break;
    }
    return u_B;
}

void calculateUncertainty() {
    std::string dataFilename;
    std::cout << "Введите имя файла с данными для расчета неопределенности: ";
    std::cin >> dataFilename;

    auto data = readAndConvertData(dataFilename);

    if (data.empty()) {
        std::cerr << "Файл с данными пуст или содержит нечисловые данные." << std::endl;
        return;
    }

    double mean = calculateMean(data);
    double uA = calculateTypeAUncertainty(data, mean);
    double uB = calculateTypeBUncertainty(-1.0, 1.0, 'U'); // Пример расчета для равномерного распределения

    std::cout << "Среднее значение: " << mean << std::endl;
    std::cout << "Неопределенность типа А: " << uA << std::endl;
    std::cout << "Неопределенность типа В (равномерное распределение): " << uB << std::endl;
}

int main() {
    std::locale::global(std::locale("")); // Устанавливаем локаль по умолчанию
    int choice;
    std::cout << "Выберите режим работы:\n1 - Расчет погрешности\n2 - Расчет неопределённости\nВведите выбор: ";
    std::cin >> choice;

    if (choice == 1) {
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
        int sampleSize = static_cast<int>(data.size());

        double quantile = quantiles.count(sampleSize) ? quantiles[sampleSize] : quantiles.rbegin()->second;
        double confidenceInterval = calculateConfidenceInterval(stdDev, quantile, sampleSize);

        std::cout << "\nСреднее значение: " << mean << std::endl;
        std::cout << "Среднеквадратичное отклонение: " << stdDev << std::endl;
        std::cout << "95% доверительный интервал: " << confidenceInterval << std::endl;
    }
    else if (choice == 2) {
        calculateUncertainty();
    }
    else {
        std::cout << "Некорректный ввод. Пожалуйста, выберите 1 или 2." << std::endl;
    }

    return 0;
}
