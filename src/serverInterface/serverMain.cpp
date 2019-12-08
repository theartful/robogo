
#include "serverInterface.hpp"

int main(int argc, char* argv[]){
    string uri = "ws://localhost:8080";
    //could take server uri from console
    if (argc == 2) {
        uri = argv[1];
    }
    ServerInterface I(uri);
    I.run();
    return 0;
}