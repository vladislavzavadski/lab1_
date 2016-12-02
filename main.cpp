#include <iostream>
#include "Server.h"

int main() {
    Server *server;


    server = new Server(30047);

    server->start();

    return 0;
}

