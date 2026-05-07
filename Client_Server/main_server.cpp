#include "TCPServer.hpp"
#include "func.hpp"
#include <sstream>
#include <iomanip>

TCPServer tcp; 

void* loop(void* m)
{
    (void)m;
    pthread_detach(pthread_self());
    
    // Параметры функции (можно задать здесь или получать от клиента)
    double a = 1.0;
    double b = 2.0;
    double c = 3.0;
    
    while(1)
    {
        std::string str = tcp.getMessage();
        if(str != "")
        {
            try {
                // Преобразуем полученное число из строки в double
                double x = std::stod(str);
                std::cout << "Received x = " << x << std::endl;
                
                // Вычисляем функцию
                double result = calculateFunction(x, a, b, c);
                
                // Формируем ответ
                std::ostringstream response;
                if (std::isnan(result))
                {
                    response << "Error: Invalid argument for function";
                }
                else
                {
                    response << std::fixed << std::setprecision(6) << result;
                }
                
                // Отправляем результат обратно клиенту
                tcp.Send(response.str());
                std::cout << "Sent result: " << response.str() << std::endl;
                
                tcp.clean();
            }
            catch (const std::exception& e)
            {
                std::cout << "Error parsing number: " << e.what() << std::endl;
                tcp.Send("Error: Invalid number format");
                tcp.clean();
            }
        }
        usleep(1000);
    }
    tcp.detach();
    return NULL;
} 

int main()
{
    std::cout << "Starting server on port 11999...\n";
    
    pthread_t msg;
    tcp.setup(11999);
    std::cout << "Server started on port 11999\n";
    std::cout << "Waiting for client connection...\n";
    
    while(1) {
        std::string client_ip = tcp.receive();
        std::cout << "Client connected from: " << client_ip << std::endl;
        
        if(pthread_create(&msg, NULL, loop, NULL) == 0)
        {
            pthread_join(msg, NULL);
        }
    }
    
    return 0;
}