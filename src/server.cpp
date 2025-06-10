#include <server.hpp>
#include <iostream>


Server::Server(const int port) : servPort(port)
{
    //TODO basic construction here
    std::cout<<"Server created"<<std::endl;
}

Server::~Server()
{
    //deconstruction here
    //disconnect
}

void Server::start()
{
    //TODO main loop for connection handling
}
