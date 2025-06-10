#include <iostream>
#include <server.hpp>

int main(){
    std::cout << "Hello World!\n";
    Server server = Server(9999); //use default redis port
    return 0;
}
