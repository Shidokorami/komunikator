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
#include <sqlite3.h>
#include <nlohmann/json.hpp>

#define MAX_BUFF_SIZE 1024

class ClientLogic {
public:
    ClientLogic(const std::string& serverAddress, int port);
    bool connectToServer();
    //std::string SendRequest(const std::string& request);
    //void Disconnect();

    bool Login(std::string username, std::string password);
    bool Register(std::string username, std::string password);
    void getGroupchatsList();
    void getMessages();
    void sendMessage(int chat_id, std::string content);
    std::string messageListener();

private:
    sqlite3* database;
    int rc;
    int SocketFD;
    std::string serverAddress;
    int port;
    sockaddr_in server_addr;
    void sendGroupchatsToDB(int chat_id, std::string chat_name);
};

#endif // CLIENTLOGIC_H