#include <server.hpp>
#include <parser.hpp>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <kvstore.hpp>
#include <util.hpp>
#include <RESPtype.hpp>
#include <commands.hpp>



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
    std::cout << "Server shutting down..." << std::endl;
    close(this->sock);
}

void Server::start()
{
    //TODO for connection IMPLEMENT HANDSHAKE and concurrency
    while (true)
    {
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
        char data;
        std::vector<std::string> command;

        while (true)
        {
            data = readByte(clientSocket);
            command = parseRESP(data, clientSocket);
            handleCommand(command);
        }
        close(clientSocket);
        return;
    } catch (const std::exception& e) {
        std::cerr << "Communication error: " << e.what() << std::endl;
        close(clientSocket);
    }
}


void Server::handleCommand(std::vector<std::string> command) //maybe change to handle resp
{
    if (command.empty())
    {
        std::cerr << "Command empty" << std::endl;
        return;
    }
    std::string cmd = command[0];
    switch (strToCmd(cmd))
    {
        case Commands::SET:
            if (command.size() != 3)
            {
                std::cerr << "Command arguments malformed" << std::endl;
                return;
            }
            kvstore.set(command[1], command[2]);
            break;

        case Commands::GET:
            if (command.size() != 2)
            {
                std::cerr << "Command arguments malformed" << std::endl;
                return;
            }
            kvstore.get(command[1]);
            break;

        case Commands::DEL:
            if (command.size() != 2)
            {
                std::cerr << "Command arguments malformed" << std::endl;
                return;
            }
            kvstore.del(command[1]);
            break;

        case Commands::EXISTS:
            if (command.size() != 2)
            {
                std::cerr << "Command arguments malformed" << std::endl;
                return;
            }
            kvstore.exists(command[1]);
            break;

        default: throw std::runtime_error("Command not non or handled"); //send error
    }

    return; //works
}
