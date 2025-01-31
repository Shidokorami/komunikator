#pragma once
#ifndef SERWER_H
#define SERWER_H
#define POLL_MAX 100

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdbool.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdexcept> 
#include <vector>
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <functional>

using json = nlohmann::json;
#define MAX_BUFFER_SIZE 1024

class Serwer{
public:
    Serwer();

    ~Serwer();
    void startServer();
    int jajeczko = 3;
private:
    std::vector<pollfd> poll_set;
    struct sockaddr_in sa;
    int serverFD;
    sqlite3* database;
    int rc;
    
    class RequestHandler{
        
    public:
        RequestHandler(Serwer& server);
        void handle(std::string requestString, int clientFD);
    private:
        Serwer& server_;
        std::unordered_map<std::string, std::function<void(json, int)>> handlers;
        std::unordered_map<int, int> connections;
        std::unordered_map<int, int> connectedClientsID;
        
        void handleLogin(json data, int clientFD);
        void handleRegister(json data, int clientFD);
        void handleReadGroupchatRequest(json data, int clientFD);
        void handleReadChatsRequest(json data, int clientFD);
        void handleReadFriendlistRequest(json data, int clientFD);
        void handleNewMessage(json data, int clientFD);
        void handleNewChatRequest(json data, int clientFD);
        void handleAddFriend(json data, int clientFD);
        void handleAddToChat(json data, int clientFD);
        void addToChat(int chat_id, std::string name, int clientFD);
        void addFriend(int sender_id, int friend_id, std::string friend_name, int clientFD);
    };

    RequestHandler requestHandler;

    
    void setupServer();
    void startPoll();
    void handleNewConnection(int serverFD, std::vector<pollfd>& poll_set);



};

#endif 