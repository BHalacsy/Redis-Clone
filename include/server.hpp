#pragma once

#include <server.hpp>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

class Server {
public:
	explicit Server(int port);
	~Server();

	void start();
	void handleComm(int clientSock, sockaddr_in& clientAddress);

private:
	int sock;
	std::string hostIP;
	int servPort;
};
