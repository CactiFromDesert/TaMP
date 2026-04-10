#pragma once

#include <iostream>
#include <cmath>
#include <iomanip>

const double PI = 3.14159265358979323846;

template <typename T>
T get_num(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
{
    T number;
    while (true)
    {
        std::cin >> number;
        if (std::cin.eof())
        {
            throw std::runtime_error("It is EOF, sorry\n");
        }
        else if (std::cin.bad())
        {
            throw std::runtime_error("WTF?!\n");
        }
        else if (std::cin.fail() || number < min || number > max)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "You are wrong; repeat please!\n";
        }
        else 
            return number;
    }
}  


double calculateFunction(double x, double a, double b, double c);

