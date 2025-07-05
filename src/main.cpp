#include <csignal>
#include <iostream>
#include <string>
#include "server.hpp"

Server* redis = nullptr;

void signalHandler(const int signal) {
    if (redis)
    {
        std::cout << "Signal received: " << signal << ". Shutting down server..." << std::endl;
        redis->stop(); // Gracefully stop the server
        delete redis;
    }
    exit(signal);
}

int main() {
    redis = new Server(6379); // Use default Redis port

    std::signal(SIGINT, signalHandler); // Handle Ctrl+C
    std::signal(SIGTERM, signalHandler); // Handle termination signal

    redis->start();

    return 0;
}
