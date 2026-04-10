#include "func.hpp"

// Функция для вычисления f(x) в зависимости от x
double calculateFunction(double x, double a, double b, double c)
{
    if (x < 0)
    {
        // Первая ветка: √(-x) + a, где x < 0
        return sqrt(-x) + a;
    }
    else if (x >= 0 && x < PI)
    {
        // Вторая ветка: arcsin(x / π) + b, где 0 <= x < π
        double argument = x / PI;
        // Проверка, что аргумент arcsin в диапазоне [-1, 1]
        if (argument < -1.0 || argument > 1.0)
        {
            std::cerr << "Ошибка: аргумент arcsin вне диапазона [-1, 1] для x = " << x << "\n";
            return NAN;
        }
        return asin(argument) + b;
    }
    else
    {
        // Третья ветка: arccos(x / π) - c, где x >= π
        double argument = x / PI;
        // Проверка, что аргумент arccos в диапазоне [-1, 1]
        if (argument < -1.0 || argument > 1.0) {
            std::cerr << "Ошибка: аргумент arccos вне диапазона [-1, 1] для x = " << x << "\n";
            return NAN;
        }
        return acos(argument) - c;
    }
}