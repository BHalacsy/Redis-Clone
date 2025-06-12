#include <server.hpp>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../third_party/Catch2/src/catch2/internal/catch_result_type.hpp"


Server::Server(const int port) : hostIP("127.0.0.1"), servPort(port)
{
    //TODO basic construction here
    std::cout<<"Server created"<<std::endl;
    this->sock = socket(AF_INET, SOCK_STREAM, 0);


    sockaddr_in sockAddress;
    sockAddress.sin_family = AF_INET;
    sockAddress.sin_addr.s_addr = inet_addr(hostIP.c_str());
    sockAddress.sin_port = htons(servPort);

    try
    {
        if (bind(this->sock, reinterpret_cast<sockaddr*>(&sockAddress), sizeof(sockAddress)) != 0) {throw std::runtime_error("server bind failed");}
        if (listen(this->sock, 5) != 0) {throw std::runtime_error("server listen failed");}
        std::cout << "Listening on: " << hostIP << " : " << servPort << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "Error caught: " << e.what() << std::endl;
    }
}

Server::~Server()
{
    //deconstruction here
    //disconnect
}

void Server::start()
{
    //TODO main loop for connection handling
    while (true)
    {
        //TODO Accept incoming multiple connection with threads
        sockaddr_in newConAddress;
        int newCon = accept(this->sock, reinterpret_cast<sockaddr*>(&newConAddress), reinterpret_cast<socklen_t*>(&newConAddress));
        this->handleComm(newCon, newConAddress);
    }
}

void Server::handleComm(int clientSocket, sockaddr_in& clientAddress)
{
    std::cout << "New Connection!" << std::endl;
}