#include "TCPClient.hpp"

TCPClient tcp;

int main()
{
   std::cout << "Trying to connect to host.docker.internal:11999...\n";
   
   if(!tcp.setup("host.docker.internal", 11999))
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
         
         srand(time(NULL));
         while(1)
         {
            int num = rand() % 1000;
            tcp.Send(std::to_string(num));
            std::string rec = tcp.receive(1024);
            if(rec != "")
               std::cout << "Server: " << rec << std::endl;
            sleep(1);
         }
      }
      sleep(2);
   }
   
   std::cout << "Connection failed\n";
   return 1;
}