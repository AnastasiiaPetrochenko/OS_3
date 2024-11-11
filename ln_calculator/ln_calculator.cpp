#include <iostream>
#include <cmath>
#include <iomanip>
#include <string> 

double taylor_ln(double x, int steps) {
    double sum = 0;
    double term = x - 1;
    for (int i = 1; i <= steps; ++i) {
        sum += (i % 2 == 1 ? 1 : -1) * term / i;
        term *= (x - 1);
    }
    return sum;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Використання: ln_calculator.exe <steps> <A> <B>" << std::endl;
        return 1;
    }

    int steps = std::stoi(argv[1]);
    double A = std::stod(argv[2]);
    double B = std::stod(argv[3]);

    // Перевірка на коректність
    if (steps < 100000) {
        std::cerr << "Кількість кроків повинна бути не меншою за 100 000." << std::endl;
        return 1;
    }
    if (A <= 0 || B <= 0) {
        std::cerr << "A і B повинні бути більшими за 0." << std::endl;
        return 1;
    }

    // Виводимо результат для A та B
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "ln(" << A << ") = " << taylor_ln(A, steps) << std::endl;
    std::cout << "ln(" << B << ") = " << taylor_ln(B, steps) << std::endl;

    return 0;
}

