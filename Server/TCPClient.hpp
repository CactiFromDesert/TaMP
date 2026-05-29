#pragma once

#ifndef TCP_CLIENT_H 
#define TCP_CLIENT_H 

#include <iostream> 
#include <signal.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // для sleep(), close()
#include <string.h>      
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector> 
#include <string>        // для std::string, std::to_string()
#include <ctime>         // для time()
#include <cstdlib>       // для rand(), srand()


void sig_exit(int s);

class TCPClient 
{ 
private:  
    int sock; 
    std::string address;
    int port; 
    struct sockaddr_in server;// Объявляем структуру, но не инициализируем здесь
    
public:
    TCPClient()
    {
        port = 0;
        address = "";
        sock = -1;
    }
    
    bool setup(std::string address, int port);
    
    bool Send(std::string data);
    
    std::string receive(int size = 1024);
    
    std::string read();
    
    void exit();
    
    bool connectToServer();
    
}; 
#endif 