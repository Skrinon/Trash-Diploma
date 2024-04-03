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

// Обновлена основная функция для обработки значений, разделенных запятыми.
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
    // Предполагая, что все числа находятся в одной строке и разделены запятыми.
    if (getline(inputFile, line)) {
        std::stringstream ss(line);
        double temp;
        while (ss >> temp) {
            data.push_back(temp);
            if (ss.peek() == ',') {
                ss.ignore(); // Игнорирование запятой
            }
            std::cout << temp << std::endl; // Отображаем номер
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