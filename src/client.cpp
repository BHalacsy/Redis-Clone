//Potentially make to comm with server, and test.
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>
#include <atomic>
#include <chrono>

#include "../include/parser.hpp"

std::atomic disconnect = false;

void handleSend(int sock)
{
    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        if (input == "!q") { disconnect = true; return; }

        std::string RESP = parseCommandToRESP(input);
        std::cout << "sending...: " << RESP << std::endl; //for debug
        send(sock, RESP.c_str(), RESP.size(), 0);
    }
}

void handleRecv(int sock)
{
    char buffer[1024];
    while (!disconnect)
    {
        std::memset(buffer, 0, sizeof(buffer));
        ssize_t recved = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (recved > 0) std::cout << "SERVER SAYS: " << buffer << std::endl;
        else
        {
            std::cout << "Server close for connections..." << std::endl;
            disconnect = true;
        }
    }
}


int main()
{
    std::string hostIP = "127.0.0.1";
    int servSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in servAddress{};
    servAddress.sin_family = AF_INET;
    servAddress.sin_addr.s_addr = inet_addr(hostIP.c_str());
    servAddress.sin_port = htons(6379);

    int connected;
    do {
        connected = connect(servSock, reinterpret_cast<sockaddr*>(&servAddress), sizeof(servAddress));
        if (connected < 0) {
            std::cout << "Server down..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    } while (connected < 0);
    //TODO implement handshake and reconnecting?
    std::cout << "Connected!" << std::endl;

    std::thread sender(handleSend, servSock);
    std::thread receiver(handleRecv, servSock);
    sender.join();
    receiver.join();

    return 0;
}



