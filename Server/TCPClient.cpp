#include "TCPClient.hpp"


bool TCPClient::setup(std::string address, int port)
{
    this->address = address;
    this->port = port;
    
    if(sock == -1)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == -1)
        {
            std::cout << "Could not create socket\n";
            return false;
        }
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    
    if(inet_addr(address.c_str()) == -1)  
    {
        struct hostent *he; 
        struct in_addr **addr_list; 

        if((he = gethostbyname(address.c_str())) == NULL)
        {
            std::cout << "Failed to resolve hostname\n";
            return false;
        }
        
        addr_list = (struct in_addr **) he->h_addr_list;
        
        for(int i = 0; addr_list[i] != NULL; i++)
        {
            server.sin_addr = *addr_list[i];
            break;
        }
    } 
    else
    {
        server.sin_addr.s_addr = inet_addr(address.c_str());
    }
    
    return true;
}

bool TCPClient::connectToServer()
{
    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        std::cout << "connect failed. Error\n";
        return false;
    }
    return true;
}

bool TCPClient::Send(std::string data)
{
    if(sock != -1)
    {
        if(send(sock, data.c_str(), data.length(), 0) < 0)
        {
            std::cout << "Send failed : " << data << std::endl;
            return false;
        }
    }
    else
        return false;
    return true;
} 

std::string TCPClient::receive(int size)
{
    char buffer[size + 1];
    memset(buffer, 0, sizeof(buffer));
    std::string reply; 

    int bytes_received = recv(sock, buffer, size, 0);
    if(bytes_received < 0)
    {
        std::cout << "receive failed!\n";
        return "";
    }
    
    buffer[bytes_received] = '\0';
    reply = buffer;
    return reply;
}    

std::string TCPClient::read()
{
    char buffer[1] = {};
    std::string reply;
    
    while (buffer[0] != '\n')
    {
        if(recv(sock, buffer, sizeof(buffer), 0) < 0)
        {
            std::cout << "receive failed!\n";
            return "";
        }
        reply += buffer[0];
    }
    return reply;
} 

void TCPClient::exit()
{
    if(sock != -1)
    {
        close(sock);
        sock = -1;
    }
}