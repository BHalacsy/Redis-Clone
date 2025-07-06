#pragma once

#include <netinet/in.h>
#include <string>
#include <vector>
#include <boost/asio/thread_pool.hpp>

#include "config.h"
#include "kvstore.hpp"
#include "pubsub.hpp"

class Server {
public:
	explicit Server();
	~Server();

	void start();
	void stop();
	void handleCommunication(int clientSock, sockaddr_in clientAddress);
	std::string handleCommand(const std::vector<std::string>& command);

private:
	int sock;
	int servPort = PORT_NUM;
	std::string hostIP = HOST_IP;

	bool running = true;
	boost::asio::thread_pool pool;

	PubSub pubsubManager;
	KVStore kvstore;
};
