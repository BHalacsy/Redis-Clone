#include <iostream>
#include <server.hpp>
#include <string>

int main(){
    Server server = Server(6379); //use default redis port
    server.start();
    std::string input = "";
    while (true)
    {
        std::cin >> input;
        if (input == "!q") {break;}
        else
        {
            //send here
        }
    }
    return 0;
}
