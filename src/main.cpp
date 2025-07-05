#include <iostream>
#include "server.hpp"
#include <string>

int main(){
    Server server = Server(6379); //use default redis port
    server.start();

    return 0;
}
