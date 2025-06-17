//Potentially make to comm with server, and test.
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <parser.hpp>

int main()
{
    std::string hostIP = "127.0.0.1";
    int conSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in connectAddress;
    connectAddress.sin_family = AF_INET;
    connectAddress.sin_addr.s_addr = inet_addr(hostIP.c_str());
    connectAddress.sin_port = htons(6379);

    connect(conSock, reinterpret_cast<sockaddr*>(&connectAddress), sizeof(connectAddress));

    std::string input;
    while (true)
    {
        std::cin >> input;
        if (input == "!q") { break; }
        std::string RESP = parseCommandToRESP(input);
        send(conSock, RESP.c_str(), RESP.size(), 0);
    }
    return 0;
}


