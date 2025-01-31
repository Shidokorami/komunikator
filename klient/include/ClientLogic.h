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

#define MAX_BUFF_SIZE 2048

class ClientLogic {
public:
    ClientLogic(const std::string& serverAddress, int port);
    bool connectToServer();

    bool Login(std::string username, std::string password);
    bool Register(std::string username, std::string password);
    void getGroupchatsList();
    void getMessages();
    void getFriendList();
    void sendMessage(int chat_id, std::string content);
    void createNewChat(std::string name);
    void addFriend(std::string name);
    void addToChat(int chat_id, std::string name);
    std::string messageListener();

private:
    sqlite3* database;
    int rc;
    int SocketFD;
    std::string serverAddress;
    int port;
    sockaddr_in server_addr;
    void sendGroupchatsToDB(int chat_id, std::string chat_name);
    void sendFriendToDB(std::string name);
};

#endif // CLIENTLOGIC_H