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
#include <stdlib.h>
#include "Socket.h"

#ifndef LABA1_SERVER_H
#define LABA1_SERVER_H

using namespace std;


class Server {

private:


    Socket *sock;

public:
    Server(int port_number) {
        sock = new Socket(port_number);
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

    void upload(int connection_d, char* fileName){
        fd_set rfDs;
        struct timeval tv;
        FD_ZERO(&rfDs);
        FD_SET(connection_d, &rfDs);

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        char *buffer = new char[1024];
        string fullPath("/home/vladislav/Универ/SPOLKS_LABS/uploads/");

        fstream file(fullPath+=fileName, ios::out);

        while (true) {
           int result = select(connection_d+1, &rfDs, &rfDs, NULL, &tv);//Допилить функцию
            if(result == 0){
                cout<<"wef";
            }
           char *buffer = sock->receiveMessage(connection_d, buffer);

            if(!strcmp(buffer, "file_end\r\n") || !strcmp(buffer, "file_end")){
                break;
            }

            file << buffer;
        }

        file.close();
        delete buffer;

    }

    void processClient(int connect_d) {
        char* message = new char[1029];

        while (1) {
            char *message = sock->receiveMessage(connect_d, message);
            char temp[1024];
            setNullTerminator(message);
            strcpy(temp, message);

            char *command = strtok(message, " ");

            if (command == NULL) {

                sock->send_message(connect_d, "Unknown command\r\n\0");

            } else if (!strcmp(command, "download")) {
                downloadFile(connect_d, strtok(NULL, " "));
            } else if(!strcmp(command, "upload")) {
                char* fileName = strtok(NULL, " ");
                upload(connect_d, fileName);
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
        //setNullTerminator(input);

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

    void downloadFile(int connect_d, char* fileName){
        ifstream file (fileName, ios::out | ios::binary);
        char buffer[4196];

        if(!file.is_open()){
            cout<<"File not found"<<endl;
            throw exception();
        }

        while(!file.eof()){
            file.read(buffer, 4196);

            sock->send_message(connect_d, buffer);
        }

        sock->send_message(connect_d, "file_end");
        file.close();
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