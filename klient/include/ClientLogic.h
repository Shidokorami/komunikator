#pragma once
#ifndef CLIENTLOGIC_H
#define CLIENTLOGIC_H

#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_BUFF_SIZE 256

class ClientLogic {
public:
    ClientLogic(const std::string& serverAddress, int port);
    bool connectToServer();
    //std::string SendRequest(const std::string& request);
    //void Disconnect();

    void Login(std::string username, std::string password);
    bool Register(std::string username, std::string password);

private:
    int SocketFD;
    std::string serverAddress;
    int port;
    sockaddr_in server_addr;
};

#endif // CLIENTLOGIC_H