#include <csignal>
#include <iostream>
#include <string>
#include "server.hpp"

Server* redis = nullptr;

void signalHandler(const int signal)
{
    if (redis)
    {
        redis->stop(); //Gracefully stop the server
        delete redis;
    }
    exit(signal);
}

int main()
{
    redis = new Server(); //Uses default Redis port (config.h to change)

    std::signal(SIGINT, signalHandler); //Handle Ctrl+C
    std::signal(SIGTERM, signalHandler); //Handle termination signal

    redis->start();

    return 0;
}
