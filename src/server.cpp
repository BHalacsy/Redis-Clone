#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <thread>
#include <format>

#include "config.h"
#include "server.hpp"
#include "parser.hpp"
#include "kvstore.hpp"
#include "util.hpp"
#include "commands.hpp"


Server::Server() : pool(POOL_SIZE), kvstore(true, "dump.rdb")
{
    std::cout << "Server launch!" << std::endl;
    this->sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in sockAddress{};
    sockAddress.sin_family = AF_INET;
    sockAddress.sin_addr.s_addr = inet_addr(hostIP.c_str());
    sockAddress.sin_port = htons(servPort);

    try
    {
        if (bind(this->sock, reinterpret_cast<sockaddr*>(&sockAddress), sizeof(sockAddress)) != 0)
        {
            throw std::runtime_error("Server bind failed");
        }
        if (listen(this->sock, 5) != 0) { throw std::runtime_error("Server listen failed"); }
        std::cout << "Listening on: " << hostIP << " : " << servPort << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Error caught: " << e.what() << std::endl;
    }
}

Server::~Server()
{
    std::cout << "Server shutting down..." << std::endl;
    pool.join();
    close(this->sock);
}

void Server::start()
{
    //TODO for connection IMPLEMENT HANDSHAKE and thread pool
    std::thread snapShotPerMinute([this]
    {
        while (running)
        {
            std::this_thread::sleep_for(std::chrono::seconds(60));
            kvstore.saveToDisk();
        }
    });

    while (running)
    {
        sockaddr_in connectionAddress{};
        socklen_t addLen = sizeof(connectionAddress);

        std::cout << "Waiting for new connections..." << std::endl;
        int connectionSock = accept(this->sock, reinterpret_cast<sockaddr*>(&connectionAddress), &addLen);
        boost::asio::post(pool, [this, connectionSock, connectionAddress]()
        {
            handleCommunication(connectionSock, connectionAddress);
        });
    }
}

void Server::stop()
{
    running = false;
}


void Server::handleCommunication(const int clientSock, sockaddr_in clientAddress)
{
    std::cout << "New connection" << std::endl;
    char buffer[4096];
    while (true)
    {
        try
        {
            //receive and parse
            const ssize_t bytesRead = recv(clientSock, &buffer, sizeof(buffer), 0);
            if (bytesRead <= 0) break;
            size_t offset = 0;
            std::vector<std::string> command = parseRESP(buffer, bytesRead, offset);

            //handle and send
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
    const std::vector arguments(command.begin() + 1, command.end());

    if (command.empty())
    {
        std::cerr << "Command empty" << std::endl;
        return "-ERR command line empty\r\n";
    }

    switch (strToCmd(command[0]))
    {
        case Commands::PING: return handlePING(arguments);
        case Commands::ECHO: return handleECHO(arguments);
        case Commands::SET: return handleSET(kvstore, arguments);
        case Commands::GET: return handleGET(kvstore, arguments);
        case Commands::DEL: return handleDEL(kvstore, arguments);
        case Commands::EXISTS: return handleEXISTS(kvstore, arguments);
        case Commands::INCR: return handleINCR(kvstore, arguments);
        case Commands::DCR: return handleDCR(kvstore, arguments);
        case Commands::EXPIRE: return handleEXPIRE(kvstore, arguments);
        case Commands::TTL: return handleTTL(kvstore, arguments);
        case Commands::FLUSHALL: return handleFLUSHALL(kvstore, arguments);
        case Commands::MGET: return handleMGET(kvstore, arguments);
        case Commands::LPUSH: return handleLPUSH(kvstore, arguments);
        case Commands::RPUSH: return handleRPUSH(kvstore, arguments);
        case Commands::LPOP: return handleLPOP(kvstore, arguments);
        case Commands::RPOP: return handleRPOP(kvstore, arguments);
        case Commands::LRANGE: return handleLRANGE(kvstore, arguments);
        case Commands::LLEN: return handleLLEN(kvstore, arguments);
        case Commands::LINDEX: return handleLINDEX(kvstore, arguments);
        case Commands::LSET: return handleLSET(kvstore, arguments);
        case Commands::LREM: return handleLREM(kvstore, arguments);
        case Commands::SADD: return handleSADD(kvstore, arguments);
        case Commands::SREM: return handleSREM(kvstore, arguments);
        case Commands::SISMEMBER: return handleSISMEMBER(kvstore, arguments);
        case Commands::SMEMBERS: return handleSMEMBERS(kvstore, arguments);
        case Commands::SCARD: return handleSCARD(kvstore, arguments);
        case Commands::SPOP: return handleSPOP(kvstore, arguments);
        case Commands::HSET: return handleHSET(kvstore, arguments);
        case Commands::HGET: return handleHGET(kvstore, arguments);
        case Commands::HDEL: return handleHDEL(kvstore, arguments);
        case Commands::HEXISTS: return handleHEXISTS(kvstore, arguments);
        case Commands::HLEN: return handleHLEN(kvstore, arguments);
        case Commands::HKEYS: return handleHKEYS(kvstore, arguments);
        case Commands::HVALS: return handleHVALS(kvstore, arguments);
        case Commands::HMGET: return handleHMGET(kvstore, arguments);
        default:
            std::cerr << "Command not handled" << std::endl;
            return std::format("-ERR unknown command '{}'", command[0]);//send error
    }
}
