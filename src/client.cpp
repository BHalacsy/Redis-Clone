//Potentially make to comm with server, and test.
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

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
        send(conSock, input.c_str(), input.size(), 0);
        //parse(input) for RESP
    }
    return 0;
}


