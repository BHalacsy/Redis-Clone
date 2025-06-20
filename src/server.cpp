#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <format>

#include <server.hpp>
#include <parser.hpp>
#include <kvstore.hpp>
#include <util.hpp>
#include <RESPtype.hpp>
#include <commands.hpp>



Server::Server(const int port) : hostIP("127.0.0.1"), servPort(port)
{
    //TODO basic construction here
    std::cout << "Server created" << std::endl;
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

[[noreturn]] void Server::start()
{
    //TODO for connection IMPLEMENT HANDSHAKE and concurrency
    while (true)
    {
        sockaddr_in connectionAddress{};
        socklen_t addLen = sizeof(connectionAddress);

        std::cout << "Waiting for new connection..." << std::endl;
        int connectionSock = accept(this->sock, reinterpret_cast<sockaddr*>(&connectionAddress), &addLen);
        std::thread worker(&Server::handleCommunication, this, connectionSock, connectionAddress);
        worker.detach();
    }
}

void Server::handleCommunication(int clientSock, sockaddr_in clientAddress)
{
    std::cout << "New connection" << std::endl;
    while (true)
    {
        try
        {
            //read and parse
            char data = readByte(clientSock);
            std::vector<std::string> command = parseRESP(data, clientSock);

            //handle and return
            std::string resp = handleCommand(command);
            send(clientSock, resp.c_str(), resp.size(), 0);
        } catch (const std::exception& e) {
            std::cerr << "Error during communication: " << e.what() << std::endl;
            break;
        }
    }
    close(clientSock);
}


std::string Server::handleCommand(const std::vector<std::string>& command) //maybe change to handle resp
{
    std::string response;
    const std::vector arguments(command.begin() + 1, command.end());

    if (command.empty())
    {
        std::cerr << "Command empty" << std::endl;
        response = "-ERR command line empty\r\n";
        return response;
    }

    switch (strToCmd(command[0]))
    {
        //TODO make commands be associated with Comands class emu thing
        case Commands::SET:
            response = handleSet(kvstore, arguments);
            break;

        case Commands::GET:
            response = handleGet(kvstore, arguments);
            break;

        case Commands::DEL:
            response = handleDel(kvstore, arguments);
            break;

        case Commands::EXISTS:
            response = handleExists(kvstore, arguments);
            break;

        default:
            std::cerr << "Command not handled" << std::endl;
            response = std::format("-ERR unknown command '{}'", command[0]);//send error
    }

    return response;
}
