#pragma once

#include <netinet/in.h>
#include <string>
#include <vector>
#include <boost/asio/thread_pool.hpp>

#include "config.h"
#include "kvstore.hpp"
#include "session.hpp"
#include "pubsub.hpp"

class Server {
public:
	explicit Server();
	~Server();

	void start(); //Sets up server loop for connection acceptance
	void stop(); //Called by main to signal graceful shutdown
	void handleCommunication(int clientSock, sockaddr_in clientAddress); //Handles accepted connection for REQ/RES loop
	std::string handleCommand(const std::vector<std::string>& command, Session* session); //Individual cmd handling, returns RESP

private:
	//Server connection values
	int sock;
	int servPort = PORT_NUM;
	std::string hostIP = HOST_IP;

	bool running = true;
	boost::asio::thread_pool pool;

	PubSub pubsubManager;
	KVStore kvstore;
};
