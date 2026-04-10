#include "func.hpp"

//setlocale(LC_ALL,"");

int main()
{
    setlocale(LC_ALL,"");
    double a, b, c;
    
    std::cout << "========================================" << "\n";
    std::cout << "Кусочная функция f(x):" << "\n";
    std::cout << "  √(-x) + a,         где x < 0" << "\n";
    std::cout << "  arcsin(x/π) + b,   где 0 ≤ x < π" << "\n";
    std::cout << "  arccos(x/π) - c,   где x ≥ π" << "\n";
    std::cout << "========================================" << "\n\n";
    
    // Ввод параметров a, b, c
    std::cout << "Введите параметр a: ";
    //std::cin >> a;
    a = get_num<double>(-10, 10);
    
    std::cout << "Введите параметр b: ";
    //std::cin >> b;
    b = get_num<double>(-10, 10);
    
    std::cout << "Введите параметр c: ";
    //std::cin >> c;
    c = get_num<double>(-10, 10);
    
    std::cout << "\nПараметры: a = " << a << ", b = " << b << ", c = " << c << "\n";
    std::cout << "========================================" << "\n";
    std::cout << std::fixed << std::setprecision(6);
    
    // Примеры вычислений
    std::cout << "\nПримеры вычислений:\n";
    
    // Пример с x = -4 
    double x1 = -4;
    double result1 = calculateFunction(x1, a, b, c);
    if (!std::isnan(result1))
    {
        std::cout << "f(" << x1 << ") = √(-(" << x1 << ")) + " << a << " = " << result1 << "\n";
    }
    
    // Пример с x = 0
    double x2 = 0;
    double result2 = calculateFunction(x2, a, b, c);
    if (!std::isnan(result2))
    {
        std::cout << "f(" << x2 << ") = arcsin(" << x2 << "/π) + " << b << " = " << result2 << "\n";
    }
    
    // Пример с x = π/2
    double x3 = PI / 2;
    double result3 = calculateFunction(x3, a, b, c);
    if (!std::isnan(result3))
    {
        std::cout << "f(" << x3 << ") = arcsin(" << x3 << "/π) + " << b << " = " << result3 << "\n";
    }
    
    // Пример с x = π
    double x4 = PI;
    double result4 = calculateFunction(x4, a, b, c);
    if (!std::isnan(result4))
    {
        std::cout << "f(" << x4 << ") = arccos(" << x4 << "/π) - " << c << " = " << result4 << "\n";
    }
    
    // Пример с x = 2π
    double x5 = 2 * PI;
    double result5 = calculateFunction(x5, a, b, c);
    if (!std::isnan(result5))
    {
        std::cout << "f(" << x5 << ") = arccos(" << x5 << "/π) - " << c << " = " << result5 << "\n";
    }
    
    std::cout << "\n========================================\n";
    
    // Интерактивный режим для вычисления любых x
    char choice;
    std::cout << "\nХотите вычислить f(x) для другого значения x? (y/exit): ";
    //std::cin >> choice;
    choice = get_num<char>();

    while (choice == 'y' || choice == 'Y')
    {
        double x;
        std::cout << "Введите x: ";
        //std::cin >> x;
        x = get_num<double>();
        
        double result = calculateFunction(x, a, b, c);
        
        if (!std::isnan(result))
        {
            std::cout << "f(" << x << ") = ";
            
            if (x < 0)
            {
                std::cout << "√(-(" << x << ")) + " << a << " = " << result;
            }
            else if (x >= 0 && x < PI)
            {
                std::cout << "arcsin(" << x << "/π) + " << b << " = " << result;
            }
            else
            {
                std::cout << "arccos(" << x << "/π) - " << c << " = " << result;
            }
            std::cout << "\n";
        }
        
        std::cout << "\nВычислить еще? (y/n): ";
        //std::cin >> choice;
        choice = get_num<char>();
    }
    
    std::cout << "\nПрограмма завершена.\n";
    return 0;
}