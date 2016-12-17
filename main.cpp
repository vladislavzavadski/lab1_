#include <iostream>
#include "Server.h"

int main() {
    Server *server;


    server = new Server(30049);

    server->start();

    return 0;
}

