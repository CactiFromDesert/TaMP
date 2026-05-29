#include "TCPServer.hpp"
#include "func.hpp"
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

TCPServer tcp;

/**
 * @brief Генерирует JSON-массив точек для построения графика функции.
 * 
 * Функция вычисляет значения f(x) на интервале [-2.0; π+0.5] 
 * и возвращает их в формате JSON для отправки клиенту.
 * 
 * @param a Коэффициент a для ветви x < 0 (√(-x) + a)
 * @param b Коэффициент b для ветви 0 ≤ x < π (arcsin(x/π) + b)
 * @param c Коэффициент c для ветви x ≥ π (arccos(x/π) - c)
 * @param numPoints Количество точек для генерации (по умолчанию 20)
 * @return std::string JSON-строка вида [[x1,y1],[x2,y2],...]
 */
std::string generatePointsJSON(double a, double b, double c, int numPoints = 20) {
    std::ostringstream json;
    json << "[";
    
    double xMin = -2.0, xMax = PI + 0.5;
    double step = (xMax - xMin) / numPoints;
    
    for (int i = 0; i <= numPoints; i++) {
        double x = xMin + i * step;
        double y = calculateFunction(x, a, b, c);
        
        if (i > 0) json << ",";
        json << "[" << std::fixed << std::setprecision(2) << x << ",";
        
        if (std::isnan(y) || std::isinf(y)) {
            json << "null";  ///< Нечисловые значения заменяем на null
        } else {
            json << std::fixed << std::setprecision(6) << y;
        }
        json << "]";
    }
    
    json << "]";
    return json.str();
}

/**
 * @brief Потоковая функция для обработки входящих соединений.
 * 
 * Работает в отдельном потоке для каждого клиента. 
 * Читает сообщения от клиента и отправляет ответы.
 * 
 * Форматы запросов:
 * - "x" → возвращает f(x) с параметрами по умолчанию (a=1, b=2, c=3)
 * - "a;b;c" → возвращает JSON со всеми точками графика
 * 
 * @param m Указатель на данные потока (не используется)
 * @return void* Всегда возвращает NULL
 */
void* loop(void* m) {
    (void)m;
    pthread_detach(pthread_self());
    
    while(1) {
        std::string str = tcp.getMessage();
        if(str != "") {
            std::cout << "Received: " << str << std::endl;
            
            try {
                // Парсим запрос: "a;b;c" или "x_value"
                std::istringstream iss(str);
                std::string token;
                std::vector<std::string> tokens;
                
                while (std::getline(iss, token, ';')) {
                    tokens.push_back(token);
                }
                
                if (tokens.size() == 1) {
                    /// Режим 1: вычисление для одного x
                    double x = std::stod(tokens[0]);
                    double result = calculateFunction(x, 1.0, 2.0, 3.0);
                    
                    std::ostringstream response;
                    if (std::isnan(result)) {
                        response << "Error";
                    } else {
                        response << std::fixed << std::setprecision(6) << result;
                    }
                    tcp.Send(response.str());
                    
                } else if (tokens.size() == 3) {
                    /// Режим 2: генерация графика с параметрами a,b,c
                    double a = std::stod(tokens[0]);
                    double b = std::stod(tokens[1]);
                    double c = std::stod(tokens[2]);
                    
                    std::cout << "Calculating with a=" << a << " b=" << b << " c=" << c << std::endl;
                    
                    std::string json = generatePointsJSON(a, b, c, 200);
                    tcp.Send(json + "\n");
                    
                    std::cout << "Sent " << json.length() << " bytes of point data\n";
                }
                
                tcp.clean();
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
                tcp.Send("Error\n");
                tcp.clean();
            }
        }
        usleep(1000);
    }
    return NULL;
}

/**
 * @brief Главная функция сервера вычислений.
 * 
 * Запускает TCP-сервер на порту 11999 и принимает входящие соединения.
 * Для каждого клиента создаётся отдельный поток.
 * 
 * @return int Всегда возвращает 0
 */
int main() {
    std::cout << "Calculation Server starting on port 11999...\n";
    std::cout << "Supports both:\n";
    std::cout << "  - Single x value\n";
    std::cout << "  - Parameters a;b;c for graph generation\n";
    
    pthread_t msg;
    tcp.setup(11999);
    std::cout << "Server started on port 11999\n";
    
    while(1) {
        std::string client_ip = tcp.receive();
        std::cout << "Client connected from: " << client_ip << std::endl;
        
        if(pthread_create(&msg, NULL, loop, NULL) == 0) {
            pthread_detach(msg);  ///< Не ждем, разрешаем множественные подключения
        }
    }
    
    return 0;
}