#pragma once

#include <netinet/in.h>
#include <string>
#include <vector>

#include "kvstore.hpp"
#include "server.hpp"

class Server {
public:
	explicit Server(int port);
	~Server();

	[[noreturn]] void start();
	void handleCommunication(int clientSock, sockaddr_in clientAddress);
	std::string handleCommand(const std::vector<std::string>& command);

private:
	int sock;
	std::string hostIP;
	int servPort;
	KVStore kvstore;
};
