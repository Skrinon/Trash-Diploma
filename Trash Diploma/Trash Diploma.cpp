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
    if (values.empty()) return std::nan("1");  // Возвращает NaN если массив пуст
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

// Функция для вычисления стандартного отклонения
double calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.size() < 2) return std::nan("1"); // Предотвращение деления на ноль
    double sum = 0.0;
    for (auto value : values) {
        sum += std::pow(value - mean, 2);
    }
    return std::sqrt(sum / (values.size() - 1));
}

// Функция для чтения квантилей Стьюдента из файла
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
        while (getline(ss, value, ',')) {  // Предполагаем, что данные разделены запятыми
            data.push_back(std::stod(value));
        }
    }
    return data;
}

// Функции для расчёта неопределённости типа A и типа B
double calculateTypeAUncertainty(const std::vector<double>& values, double mean) {
    double sum = 0.0;
    for (auto value : values) {
        sum += std::pow(value - mean, 2);
    }
    return std::sqrt(sum / (values.size() * (values.size() - 1)));
}

double calculateTypeBUncertainty(double a_minus, double a_plus, char distributionType) {
    double u_B = 0.0;
    switch (distributionType) {
    case 'U':
        u_B = (a_plus - a_minus) / std::sqrt(12);
        break;
    case 'T':
        u_B = (a_plus - a_minus) / std::sqrt(24);
        break;
    case 'R':
    {
        double beta = 0.5;  // Инициализация переменной в блоке
        u_B = (a_plus - a_minus) / (std::sqrt(24) * std::sqrt(1 + beta * beta));
    }
    break;
    case 'E':
    {
        double lambda = 1.0;  // Scale parameter
        double x = (a_plus + a_minus) / 2;
        u_B = std::sqrt(((a_plus - x) * (x - a_minus) - ((a_plus - 2 * x + a_minus) / lambda)));
    }
    break;
    }
    return u_B;
}

// Функция для расчёта коэффициента корреляции
double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    double mean_x = calculateMean(x);
    double mean_y = calculateMean(y);
    double sum_xy = 0.0;
    double sum_xx = 0.0;
    double sum_yy = 0.0;

    for (size_t i = 0; i < x.size(); ++i) {
        sum_xy += (x[i] - mean_x) * (y[i] - mean_y);
        sum_xx += (x[i] - mean_x) * (x[i] - mean_x);
        sum_yy += (y[i] - mean_y) * (y[i] - mean_y);
    }

    double denominator = std::sqrt(sum_xx) * std::sqrt(sum_yy);
    if (denominator == 0) return std::nan("1");

    return sum_xy / denominator;
}

// Функция для расчёта суммарной стандартной неопределенности
double calculateCombinedUncertainty(const std::vector<double>& x, const std::vector<double>& y, double uAX, double uAY) {
    double dydx = 1.0;  // Предположение линейной зависимости
    double cov_xy = calculateCorrelation(x, y) * uAX * uAY;

    double combinedUncertaintySquared = dydx * dydx * (uAX * uAX + uAY * uAY) + 2 * dydx * dydx * cov_xy;
    return std::sqrt(combinedUncertaintySquared);
}


int main() {
    std::locale::global(std::locale("")); // Установка стандартной локали
    int choice;
    std::cout << "Выберите режим работы:\n1 - Расчет погрешности\n2 - Расчет неопределённости\nВведите выбор: ";
    std::cin >> choice;

    if (choice == 1) {
        // Расчет погрешности
        std::string dataFilename, quantilesFilename;
        std::cout << "Введите имя файла с данными: ";
        std::cin >> dataFilename;
        std::cout << "Введите имя файла с квантилями Стьюдента: ";
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

        if (quantiles.count(sampleSize) > 0) {
            double quantile = quantiles[sampleSize];
            double confidenceInterval = calculateConfidenceInterval(stdDev, quantile, sampleSize);

            std::cout << "\nСреднее значение: " << mean << std::endl;
            std::cout << "Среднеквадратичное отклонение: " << stdDev << std::endl;
            std::cout << "95% доверительный интервал: " << confidenceInterval << std::endl;
        }
        else {
            std::cerr << "Для данного размера выборки квантиль отсутствует." << std::endl;
        }
    }
    else if (choice == 2) {
        // Расчет неопределённости
        std::string dataFilenameX, dataFilenameY, quantilesFilename;
        std::cout << "Введите имя файла с данными X: ";
        std::cin >> dataFilenameX;
        std::cout << "Введите имя файла с данными Y: ";
        std::cin >> dataFilenameY;
        std::cout << "Введите имя файла с квантилями Стьюдента: ";
        std::cin >> quantilesFilename;

        auto dataX = readAndConvertData(dataFilenameX);
        auto dataY = readAndConvertData(dataFilenameY);
        auto quantiles = readQuantiles(quantilesFilename);

        if (dataX.empty() || dataY.empty()) {
            std::cerr << "Один из файлов с данными пуст или содержит нечисловые данные." << std::endl;
            return 1;
        }

        double meanX = calculateMean(dataX);
        double meanY = calculateMean(dataY);
        double uAX = calculateTypeAUncertainty(dataX, meanX);
        double uAY = calculateTypeAUncertainty(dataY, meanY);
        double uBX = calculateTypeBUncertainty(-1.0, 1.0, 'U'); // Пример для X
        double uBY = calculateTypeBUncertainty(-1.0, 1.0, 'U'); // Пример для Y

        double correlation = calculateCorrelation(dataX, dataY);
        double combinedUncertainty = calculateCombinedUncertainty(dataX, dataY, uAX, uAY);

        int degreesOfFreedom = dataX.size() - 1; // Простое предположение
        double k = quantiles.count(degreesOfFreedom) > 0 ? quantiles[degreesOfFreedom] : 2.0; // Предположим доверительный интервал 95%
        double expandedUncertainty = calculateConfidenceInterval(combinedUncertainty, k, dataX.size());

        std::cout << "Среднее значение X: " << meanX << std::endl;
        std::cout << "Среднее значение Y: " << meanY << std::endl;
        std::cout << "Неопределенность типа А для X: " << uAX << std::endl;
        std::cout << "Неопределенность типа В для X: " << uBX << std::endl;
        std::cout << "Неопределенность типа А для Y: " << uAY << std::endl;
        std::cout << "Неопределенность типа В для Y: " << uBY << std::endl;
        std::cout << "Корреляция между X и Y: " << correlation << std::endl;
        std::cout << "Суммарная неопределенность: " << combinedUncertainty << std::endl;
        std::cout << "Расширенная неопределенность: " << expandedUncertainty << " при доверительном интервале 95%" << std::endl;
    }
    else {
        std::cout << "Некорректный ввод. Пожалуйста, выберите 1 или 2." << std::endl;
    }

    return 0;
}
