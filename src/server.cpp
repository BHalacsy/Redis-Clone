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
#include "pubsub.hpp"
#include "session.hpp"


Server::Server() : pool(POOL_SIZE), kvstore(true, SAVEFILE_PATH, KEY_LIMIT) //Config.h
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
    std::thread snapShotPerMinute([this]
    {
        while (running)
        {
            std::this_thread::sleep_for(std::chrono::seconds(SNAP_TIMER));
            kvstore.saveToDisk();
        }
    });

    while (running)
    {
        sockaddr_in connectionAddress{};
        socklen_t addLen = sizeof(connectionAddress);

        std::cout << "Waiting for new connections..." << std::endl;
        int connectionSock = accept(this->sock, reinterpret_cast<sockaddr*>(&connectionAddress), &addLen);
        if (connectionSock == -1) break;
        boost::asio::post(pool,[this, connectionSock, connectionAddress]() {
            handleCommunication(connectionSock, connectionAddress);
        });
    }
    snapShotPerMinute.join();
}

void Server::stop()
{
    running = false;
}


void Server::handleCommunication(const int clientSock, const sockaddr_in clientAddress)
{
    std::cout << "New connection from sock: " << clientSock << std::endl;

    const auto session = new Session{ //TODO maybe replace with smart ptr
            .clientSock = clientSock,
            .clientAddress = inet_ntoa(clientAddress.sin_addr)
    };

    char buffer[4096]; //Can be any number really
    while (true)
    {
        std::string resp;
        try
        {
            //receive
            const ssize_t bytesRead = recv(clientSock, &buffer, sizeof(buffer), 0);
            if (bytesRead <= 0) break;

            //append to any halved commands, parse all in pipeline return cmd, get not processed commands for next recv
            session->partialCommand.append(buffer, bytesRead);
            size_t offset = 0;
            std::vector<std::vector<std::string>> commands = parseRESPPipeline(session->partialCommand.c_str(), session->partialCommand.size(), offset);
            session->partialCommand = session->partialCommand.substr(offset);

            //handle and send
            for (const auto& command : commands)
            {
                resp += handleCommand(command, session);
            }
            send(clientSock, resp.c_str(), resp.size(), 0);
        } catch (const std::exception& e) {
            std::cerr << "Error during communication: " << e.what() << std::endl;
            break;
        }
    }
    pubsubManager.unsubscribeAll(session->clientSock);
    std::cout << "Closed connection from sock: " << clientSock << std::endl;
    close(clientSock);
    delete session;
}


std::string Server::handleCommand(const std::vector<std::string>& command, Session* session)
{
    const std::vector arguments(command.begin() + 1, command.end());

    if (command.empty())
    {
        return "-ERR command line empty\r\n";
    }
    const Commands cmd = strToCmd(command[0]);
    if (session->transActive && cmd != Commands::EXEC && cmd != Commands::DISCARD)
    {
        session->transQueue.push_back(command);
        return "+QUEUED\r\n";
    }

    switch (cmd)
    {
        case Commands::PING: return handlePING(arguments);
        case Commands::ECHO: return handleECHO(arguments);
        case Commands::DEL: return handleDEL(kvstore, arguments);
        case Commands::EXISTS: return handleEXISTS(kvstore, arguments);
        case Commands::FLUSHALL: return handleFLUSHALL(kvstore, arguments);

        case Commands::SET: return handleSET(kvstore, arguments);
        case Commands::GET: return handleGET(kvstore, arguments);
        case Commands::INCR: return handleINCR(kvstore, arguments);
        case Commands::DCR: return handleDCR(kvstore, arguments);
        case Commands::INCRBY: return handleINCRBY(kvstore, arguments);
        case Commands::DCRBY: return handleDCRBY(kvstore, arguments);
        case Commands::MGET: return handleMGET(kvstore, arguments);

        case Commands::EXPIRE: return handleEXPIRE(kvstore, arguments);
        case Commands::TTL: return handleTTL(kvstore, arguments);
        case Commands::PERSIST: return handlePERSIST(kvstore, arguments);

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
        case Commands::HGETALL: return handleHGETALL(kvstore, arguments);

        case Commands::PUBLISH: return handlePUBLISH(pubsubManager, arguments);
        case Commands::SUBSCRIBE: return handleSUBSCRIBE(pubsubManager, arguments, session->clientSock);
        case Commands::UNSUBSCRIBE: return handleUNSUBSCRIBE(pubsubManager, arguments, session->clientSock);

        case Commands::MULTI: return handleMULTI(session, arguments);
        case Commands::DISCARD: return handleDISCARD(session, arguments);
        case Commands::EXEC:
            {
                //Done here because I would need to include server.cpp in commands.cpp for recursive calls to handleCommand
                if (!session->transActive)
                {
                    return "-ERR EXEC without MULTI\r\n";
                }
                const auto queue = std::move(session->transQueue);

                session->transActive = false; //Must so EXEC-ed commands are not queued again
                session->transQueue.clear();

                std::string resp = "*" + std::to_string(queue.size()) + "\r\n";

                for (const auto& i : queue)
                {
                    assert(strToCmd(i[0]) != Commands::EXEC && "EXEC should never be in transaction queue!");
                    resp += handleCommand(i, session);
                }
                return resp;
            }

        case Commands::CONFIG: return handleCONFIG(arguments);
        case Commands::TYPE: return handleTYPE(kvstore, arguments);
        case Commands::SAVE: return handleSAVE(kvstore, arguments);
        default:
            std::cerr << "Command not handled: " << command[0] << std::endl;
            return std::format("-ERR unknown command '{}'", command[0]);
    }
}