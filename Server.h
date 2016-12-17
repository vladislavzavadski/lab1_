//
// Created by vladislav on 03.11.16.
//
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <zconf.h>
#include <fstream>
#include <stdio.h>
#include "Socket.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#ifndef LABA1_SERVER_H
#define LABA1_SERVER_H
#define BUFFER_SIZE 4096

using namespace std;


class Server {

private:


    Socket *sock;
    void *buffer = malloc(BUFFER_SIZE);

public:
    Server(int port_number) {
        sock = new Socket(port_number);
    }

    ~Server(){
        free(buffer);
    }

    void start() {

        while (1) {
            int connect_descriptor = waitClientConnecting();

          //  if(!fork()) {
                processClient(connect_descriptor);
               // _exit(0);
            //}
        }
    }

private:


    void processClient(int connect_d) {
        char* message = new char[BUFFER_SIZE];

        while (1) {
            char *message;
            try{
                message = sock->receiveMessage(connect_d, message);
            }
            catch (exception ex){
                close(connect_d);
                break;
            }

            char temp[1024];
            setNullTerminator(message);
            strcpy(temp, message);

            char *command = strtok(message, " ");

            if (command == NULL) {

                sock->send_message(connect_d, "Unknown command\r\n\0");

            } else if (!strcmp(command, "download")) {
                try {
                    char* fileName = strtok(NULL, " ");
                    char* bytesCountStr = strtok(NULL, " ");
                    long bytesSaved;
                    if(bytesCountStr == NULL) {
                        bytesSaved = 0;
                    } else{
                        bytesSaved = atol(bytesCountStr);
                    }
                    downloadFile(connect_d, fileName, bytesSaved);
                }
                catch (exception ex){
                    close(connect_d);
                    break;
                }
            }else
             {

                char *response = processCommand(temp);

                if (response == NULL) {
                    close(connect_d);
                    cout<<"Connection closed"<<endl;
                    break;
                } else{
                    sock->send_message(connect_d, response);
                }
            }
        }
        delete message;
    }



    char* processCommand(char* input){

        if(strlen(input)==0){
            return input;
        } else if(!strcmp(input, "close")){
            return NULL;
        }

        char* command = strtok(input, " ");

        if(!strcmp(command, "echo")){
            return strcat(strtok(NULL, ""), "\r\n\0");
        } else if(!strcmp(command, "time")){
            return strcat(getServerTime(), "\r\n\0");
        } else {
            return "Unknown command\r\n\0";
        }
    }

    void setNullTerminator(char* string){

        for(int i=0;i<1024;i++){

            if(string[i]=='\r'){
                string[i] = '\0';
                return;
            }
        }
    }

    char* getServerTime(){
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        return asctime (timeinfo);
    }

    void downloadFile(int connect_d, char* fileName, long loadedSize){
        FILE *file = fopen(fileName, "rb");

        char buf2[BUFFER_SIZE];

        if(file == NULL){
            sock->send_message(connect_d, "-1");
            throw exception();
        }

        fseek(file, 0, SEEK_END);

        long fileSize = ftell(file);

        fseek(file, loadedSize, SEEK_SET);

        sprintf(buf2, "%ld", fileSize);

        sock->send_message(connect_d, buf2);

        while(!feof(file)){

            fread(buffer, BUFFER_SIZE, sizeof(char), file);

            try {
                sock->send_message(connect_d, buffer, BUFFER_SIZE);

            }

            catch (exception ex){
                fclose(file);
                throw exception();
            }

        }
        fclose(file);
    }

    int waitClientConnecting(){
        sockaddr_storage client_addr;
        unsigned int address_size = sizeof(client_addr);

        int connect_d = accept(sock->getSocket_d(), (struct sockaddr*)&client_addr, &address_size);

        if(connect_d == -1){
            throw exception();
        }

        return connect_d;

    }
};


#endif //LABA1_SERVER_H
