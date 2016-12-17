//
// Created by vladislav on 05.11.16.
//

#ifndef LABA1_SOCKET_H
#define LABA1_SOCKET_H


#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>

using namespace std;

class Socket {
private:
    int socket_d;
    int socketBufferSize;
public:
    Socket(int port_number, int bufferSize = 64000){
        socketBufferSize = bufferSize;
        socket_d = socket(PF_INET, SOCK_STREAM, 0);

        if(socket_d == -1){
            throw exception();
        }

        socklen_t sock_len = sizeof(socketBufferSize);

        if(setsockopt(socket_d, SOL_SOCKET, SO_SNDBUF, &bufferSize, sock_len) == -1){
            throw exception();
        }

        sockaddr_in name;

        name.sin_family = PF_INET;
        name.sin_port = (in_port_t)htons(port_number);
        name.sin_addr.s_addr = htonl(INADDR_ANY);

        int c = bind(socket_d, (struct sockaddr*) &name, sizeof(name));

        if(c == -1){
            throw exception();
        }

        if(listen(socket_d, 10) == -1){
            throw exception();
        }
    }



    void send_message(int connect_descriptior, char* message){

        if(send(connect_descriptior, message, strlen(message), 0) == -1){
            throw exception();
        }

    }

    void send_message(int connect_descriptior, void * message, int bufSize){

        if(send(connect_descriptior, message, bufSize, 0) == -1){
            throw exception();
        }

    }

    char* receiveMessage(int connect_descriptor, char* inputBuffer){

        char* buffer = new char[1024];
        if(recv(connect_descriptor, buffer, 1024, 0) <= 0){
            cout<<errno;
            throw exception();
        }

        return buffer;
    }

    int getSocket_d(){
        return socket_d;
    }
};



#endif //LABA1_SOCKET_H
