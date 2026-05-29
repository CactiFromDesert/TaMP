#include "TCPClient.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

TCPClient tcp;

int main()
{
   std::cout << "Trying to connect to localhost:11999...\n";
   
   if(!tcp.setup("localhost", 11999))  
   {
      std::cout << "Setup failed\n";
      return 1;
   }
   
   for(int i = 0; i < 5; i++)
   {
      std::cout << "Attempt " << i+1 << "...\n";
      if(tcp.connectToServer())
      {
         std::cout << "Connected!\n";
         std::cout << "Server will calculate function f(x) where:\n";
         std::cout << "  if x < 0:     f(x) = √(-x) + a\n";
         std::cout << "  if 0 <= x < π: f(x) = arcsin(x/π) + b\n";
         std::cout << "  if x >= π:    f(x) = arccos(x/π) - c\n\n";
         
         srand(time(NULL));
         while(1)
         {
            // Генерируем случайное число в диапазоне [-10, 20]
            // чтобы задействовать все ветки функции
            int num = (rand() % 310) - 10; // от -10 до 300
            // или небольшой диапазон: int num = (rand() % 40) - 10; // от -10 до 30
            
            std::cout << "\nSending x = " << num << " to server...\n";
            tcp.Send(std::to_string(num));
            
            std::string rec = tcp.receive(1024);
            if(rec != "")
            {
               std::cout << "Server result: f(" << num << ") = " << rec << std::endl;
            }
            else
            {
               std::cout << "No response from server\n";
               break;
            }
            
            sleep(2); // Пауза между запросами
         }
      }
      sleep(2);
   }
   
   std::cout << "Connection failed\n";
   return 1;
}