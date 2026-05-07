#include "TCPServer.hpp"
#include "func.hpp"
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

TCPServer tcp;

// Функция для генерации JSON с точками графика
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
            json << "null";
        } else {
            json << std::fixed << std::setprecision(6) << y;
        }
        json << "]";
    }
    
    json << "]";
    return json.str();
}

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
                    // Старый формат - одно значение x
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
                    // Новый формат - параметры a;b;c
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
            pthread_detach(msg);  // Не ждем, разрешаем множественные подключения
        }
    }
    
    return 0;
}