#ifndef CONFIG_H
#define CONFIG_H

//Maybe change to non const and let configuring through CLI (CONFIG)

constexpr auto SAVEFILE_PATH = "dump.rdb"; //Can be anything really
constexpr int POOL_SIZE = 16; //Thread count
constexpr int PORT_NUM = 6379; //Default redis port
constexpr auto HOST_IP = "127.0.0.1";

constexpr int SNAP_TIMER = 60; //Save every x seconds


#endif
