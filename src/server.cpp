#include <server.hpp>
#include <parser.hpp>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>



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
    catch (const std::exception& e)
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
        sockaddr_in connectionAddress;
        socklen_t addLen = sizeof(connectionAddress);

        std::cout << "Waiting for new connection..." << std::endl;
        int connectionSock = accept(this->sock, reinterpret_cast<sockaddr*>(&connectionAddress), &addLen);
        std::thread worker(&Server::handleCommunication, this, connectionSock, connectionAddress);
        worker.detach();
    }
}

void Server::handleCommunication(int clientSocket, sockaddr_in clientAddress)
{
    try
    {
        std::cout << "New connection" << std::endl;
        //change to read in byte by byte and stop when end of line
        char data[2048]; //subject to change?
        std::string readBuffer;
        long recved;

        while (true)
        {
            recved = recv(clientSocket, data, sizeof(data), 0);
            if (recved <= 0) break;



            std::string retString(data, recved);
            std::cout << "Received: " << retString << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Communication error: " << e.what() << std::endl;
    }


    return;
}


void Server::handleCommand(std::vector<std::string> command)
{
    return;
}
