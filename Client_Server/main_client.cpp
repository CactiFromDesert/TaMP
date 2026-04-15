#include "TCPClient.hpp"

TCPClient tcp;

void sig_exit(int s)
{
   (void)s;
   tcp.exit();
   exit(0);
}

int main(int argc, char *argv[])
{
   (void)argc;
   (void)argv;
   
   signal(SIGINT, sig_exit);
   
   if(!tcp.setup("127.0.0.1", 11999))
   {
      std::cout << "Setup failed!\n";
      return 1;
   }
   
   if(!tcp.connectToServer())
   {
      std::cout << "Connection failed!\n";
      return 1;
   }
   
   srand(time(NULL));
   
   while(1)
   {
      int random_num = rand() % 25000;
      tcp.Send(std::to_string(random_num));
      
      std::string rec = tcp.receive(1024);
      
      if(rec != "")
      {
         std::cout << "Server Response: " << rec << std::endl;
      }
      
      sleep(1);
   }
   
   return 0;
}