#include "TCPServer.hpp"

TCPServer tcp; 

void* loop(void* m)
{
    (void)m;  // Убираем предупреждение о неиспользуемом параметре
    pthread_detach(pthread_self());
    while(1)
    {
        srand(time(NULL));
        char ch = 'a' + rand() % 26;
        std::string s(1, ch);
        std::string str = tcp.getMessage();
        if(str != "")
        {
            std::cout << "Message: " << str << '\n';
            tcp.Send("[client message: " + str + "] " + s);
            tcp.clean();
        }
        usleep(1000);
    }
    tcp.detach();
    return NULL;
} 

int main()
{
    pthread_t msg;
    tcp.setup(11999);
    std::cout << "Server started on port 11999\n";
    std::string client_ip = tcp.receive();  // Получаем IP клиента
    std::cout << "Client connected from: " << client_ip << std::endl;
    
    if(pthread_create(&msg, NULL, loop, NULL) == 0)
    {
        pthread_join(msg, NULL);  // Ждем завершения потока
    }
    
    return 0;
}