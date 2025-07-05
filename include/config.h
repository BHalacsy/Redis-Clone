#ifndef CONFIG_H
#define CONFIG_H

//Maybe change to non const and let configuring through CLI
constexpr auto SAVEFILE_PATH = "dump.rdb";
constexpr int POOL_SIZE = 8;
constexpr int PORT_NUM = 6379;
constexpr auto HOST_IP = "127.0.0.1";

constexpr int SNAP_TIMER = 60; //seconds


#endif
