#pragma once


class Server {
public:
	explicit Server(int port);
	~Server();

	void start();

private:
	int servPort;
};
