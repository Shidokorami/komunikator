#include "../../include/Serwer.h"
#include "PackersServer.h"
#define MAX_LISTEN 100

Serwer::Serwer() : requestHandler(*this){

    startServer();

}

Serwer::~Serwer(){
    if (serverFD >= 0) {
        close(serverFD);
        std::cout << "Gniazdo zamkniete." << std::endl;
    }
};

void Serwer::startServer(){
    setupServer();
    startPoll();

};

void Serwer::setupServer(){
    rc = sqlite3_open("./resources/resource/serverDB.db", &database);

    if (rc != SQLITE_OK) {
    std::cerr << "Błąd otwierania bazy danych: " << sqlite3_errmsg(database) << std::endl;

    }
    serverFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverFD  == -1){
        throw std::runtime_error("Blad tworzenia gniazda!");

    }
    else{
        std::cout << "Utworzono gniazdo..." << std::endl;
    }

    memset(&sa, 0, sizeof sa);

    sa.sin_family = AF_INET;
    sa.sin_port = htons(5000);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    fcntl(serverFD, F_SETFL, fcntl(serverFD, F_GETFL) | O_NONBLOCK);

    if(bind(serverFD, (struct sockaddr *)&sa, sizeof sa) == -1){
        throw std::runtime_error("Blad wiazania gniazda!");


    }
    else{
        std::cout << "Zwiazanie gniazda..." << std::endl;
    }

    if(listen(serverFD, MAX_LISTEN) == -1){
        throw std::runtime_error("Blad nasluchiwania!");

    }
    else{
        std::cout << "Rozpoczeto nasluchiwanie..." << std::endl;
    
    }

    pollfd server_pollfd = { serverFD, POLLIN, 0};
    poll_set.push_back(server_pollfd);

};

void Serwer::startPoll(){
    int n = 0;
    char buffer[MAX_BUFFER_SIZE];
    while (true)
    {
        n = poll(poll_set.data(), poll_set.size(), -1);
        if (n < 0) {
            std::cerr << "Poll failed!" << std::endl;
            break;
        }

        for (size_t i = 0; i < poll_set.size(); i++){
            if(poll_set[i].revents & POLLIN) 
            {
                if (poll_set[i].fd == serverFD)
                {
                    handleNewConnection(serverFD, poll_set);
                }

                else
                {
                    int clientFD = poll_set[i].fd;
                    ssize_t bytesRecv = recv(clientFD, buffer, sizeof (buffer), 0);
                    if(bytesRecv <=0 ){
                        std::cout << "ID: " << i << std::endl;
                        close(clientFD);
                        poll_set.erase(poll_set.begin() + i);
                        std::cout << "Connection closed..." << std::endl;
                    }

                    buffer[bytesRecv] = '\0';
                    std::string requestString(buffer);
                    requestHandler.handle(requestString, clientFD);
                }
            }
        }
    }
}

void Serwer::handleNewConnection(int serverFD, std::vector<pollfd>& poll_set){
    struct sockaddr_in ca;

    socklen_t clientLen = sizeof(ca);

    int clientFD = accept(serverFD, (struct sockaddr*)&ca, &clientLen);
    std::cout << "FD: " << clientFD << std::endl;
    if(clientFD < 0)
    {
        std::cerr << "Error while trying to establish connection!" << std::endl;
    }

    fcntl(clientFD, F_SETFL, fcntl(clientFD, F_GETFL) | O_NONBLOCK);

    pollfd clientPoll;

    clientPoll = { clientFD, POLLIN, 0};
    poll_set.push_back(clientPoll);
    std::cout << "Established connection..." << std::endl;

}


Serwer::RequestHandler::RequestHandler(Serwer& server)
: server_(server){
    handlers["login"] = [this](json data, int clientFD) { this->handleLogin(data, clientFD);};
    handlers["register"] = [this](json data, int clientFD) { this->handleRegister(data, clientFD);};
    handlers["read_groupchat_list"] = [this](json data, int clientFD) { this->handleReadGroupchatRequest(data, clientFD);};
    handlers["read_messages"] = [this](json data, int clientFD) { this->handleReadChatsRequest(data, clientFD);};
    handlers["incoming_message"] = [this](json data, int clientFD) { this->handleNewMessage(data, clientFD);};
    handlers["create_group"] = [this](json data, int clientFD) { this->handleNewChatRequest(data, clientFD);};
    handlers["read_friendlist"] = [this](json data, int clientFD) { this->handleReadFriendlistRequest(data, clientFD);};
    handlers["add_friend"] = [this](json data, int clientFD) { this->handleAddFriend(data, clientFD);};
    handlers["add_to_chat"] = [this](json data, int clientFD) { this->handleAddToChat(data, clientFD);};
}

void Serwer::RequestHandler::handle(std::string requestString, int clientFD){
    json request = json::parse(requestString);
    std::string requestType = request["request_type"];
    json data = request["data"];

    if(handlers.find(requestType) != handlers.end()){
        handlers[requestType](data, clientFD);
    }
    else
    {
        std::cout << "Unknown request type!" << std::endl;
    }

}

void Serwer::RequestHandler::handleLogin(json data, int clientFD){
    std::string username = data["username"];
    std::string password = data["password"];
    
    
    std::cout << "US: " << username << ", PASS: " << password << std::endl;

    sqlite3_stmt* stmt;
    std::string sql = "SELECT ID FROM USERS WHERE USERNAME = ? AND PASSWORD = ?";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    if (server_.rc!= SQLITE_OK) {
    std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(server_.database) << std::endl;
    return;
    }

    server_.rc = sqlite3_bind_text(stmt, 1, username.c_str(),-1, SQLITE_STATIC);
    server_.rc = sqlite3_bind_text(stmt, 2, password.c_str(),-1, SQLITE_STATIC);

    int user_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user_id = sqlite3_column_int(stmt, 0);
    }

    std::string mess;
    if (user_id != -1) {
        mess = packLoginSuccess(true);
        connections[clientFD] = user_id;
        connectedClientsID[user_id] = clientFD;
        
    } 
    else 
    {
        mess = packLoginSuccess(false);
    }

    send(clientFD, mess.c_str(), mess.size(), 0);

}

void Serwer::RequestHandler::handleRegister(json data, int clientFD){
    std::string username = data["username"];
    std::string password = data["password"];
    
    
    std::cout << "US: " << username << ", PASS: " << password << std::endl;

    sqlite3_stmt* stmt;
    std::string sql = "SELECT EXISTS(SELECT 1 FROM USERS WHERE USERNAME = ?)";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    if (server_.rc!= SQLITE_OK) {
    std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(server_.database) << std::endl;
    return;
    }

    server_.rc = sqlite3_bind_text(stmt, 1, username.c_str(),-1, SQLITE_STATIC);

    int exists = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0);  
    }

    std::string mess;

    if (!exists){
        sql = "INSERT INTO USERS(username, password) VALUES (?, ?)";
        server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

        if (server_.rc!= SQLITE_OK) {
        std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(server_.database) << std::endl;
        return;
        }

        server_.rc = sqlite3_bind_text(stmt, 1, username.c_str(),-1, SQLITE_STATIC);
        server_.rc = sqlite3_bind_text(stmt, 2, password.c_str(),-1, SQLITE_STATIC);
        server_.rc = sqlite3_step(stmt);
        if(server_.rc == SQLITE_DONE){
            mess = "Succesfully registered";
        }
        else{
            mess = "Error registering";
        }
    }

    else{
        mess = "User already exists";
    }

    send(clientFD, mess.c_str(), mess.size(), 0);
}

void Serwer::RequestHandler::handleReadGroupchatRequest(json data, int clientFD){
    sqlite3_stmt* stmt;
    int clientID = connections[clientFD];
    std::string sql = "SELECT g.chat_name, g.chat_id FROM GROUPCHATS g JOIN USERS_IN_GROUPCHAT ug on g.chat_id = ug.chat_id WHERE ug.user_id = ?";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    if (server_.rc!= SQLITE_OK) {
    std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(server_.database) << std::endl;
    return;
    }
    server_.rc = sqlite3_bind_int(stmt, 1, clientID);

    json sendData;
    sendData["request_type"] = "groupchats";
    sendData["data"] = json::array();

    while ((server_.rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        json chat;
        const unsigned char*  chat_name = sqlite3_column_text(stmt, 0);
        int chat_id = sqlite3_column_int(stmt, 1);
        std::string chatNameStr(reinterpret_cast<const char*>(chat_name));
        std::cout << "ID: " << chat_id << " NAME: " << chatNameStr <<  std::endl;
        chat["chat_id"] = chat_id;
        chat["chat_name"] = chatNameStr;
        sendData["data"].push_back(chat);
    }
    send(clientFD, sendData.dump().c_str(), sendData.dump().size(), 0);    
}

void Serwer::RequestHandler::handleReadChatsRequest(json data, int clientFD){
    int chat_id = data.value("chat_id", -1);
    sqlite3_stmt* stmt;
    std::string sql = "SELECT m.MESSAGE_ID, u.USERNAME, m.CONTENT FROM MESSAGES m JOIN USERS u ON m.SENDER_ID = u.ID WHERE m.CHAT_ID = ?;";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    server_.rc = sqlite3_bind_int(stmt, 1, chat_id);

    json sendData;
    sendData["request_type"] = "message_for_groupchat";
    sendData["data"] = json::array();
    while ((server_.rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        json mess;

        int mess_id = sqlite3_column_int(stmt, 0);
        const unsigned char*  sender_name = sqlite3_column_text(stmt, 1);
        const unsigned char*  content = sqlite3_column_text(stmt, 2);

        std::string senderStr(reinterpret_cast<const char*>(sender_name));
        std::string contentStr(reinterpret_cast<const char*>(content));
        
        mess["message_id"] = mess_id;
        mess["sender"] = senderStr;
        mess["content"] = contentStr;

        sendData["data"].push_back(mess);
    }

    std::string finalData = sendData.dump();
    send(clientFD, finalData.c_str(), finalData.size(), 0);

}

void Serwer::RequestHandler::handleNewMessage(json data, int clientFD){
    int chat_id = data.value("chat_id", -1);
    std::string content = data.value("content", "");
    int sender_id = connections[clientFD];

    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO MESSAGES(chat_id, sender_id, content) VALUES (?,?,?);";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    server_.rc = sqlite3_bind_int(stmt, 1, chat_id);
    server_.rc = sqlite3_bind_int(stmt, 2, sender_id);
    server_.rc = sqlite3_bind_text(stmt, 3, content.c_str(), -1, SQLITE_STATIC );

    server_.rc = sqlite3_step(stmt);

    if (server_.rc != SQLITE_DONE) {
    std::cerr << "Błąd insertu rekordu: " << sqlite3_errmsg(server_.database) << std::endl;
    }
    
    long long messageId = sqlite3_last_insert_rowid(server_.database);
    sqlite3_finalize(stmt);

    std::string sqlGetName = "SELECT USERNAME FROM USERS WHERE ID = ?;";
    server_.rc = sqlite3_prepare_v2(server_.database, sqlGetName.c_str(), -1, &stmt, nullptr);
    server_.rc = sqlite3_bind_int(stmt, 1, sender_id);

    std::string sender_name;
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* name = sqlite3_column_text(stmt, 0);
        sender_name = std::string(reinterpret_cast<const char*>(name));
    } 
    sqlite3_finalize(stmt);

    std::string sqlSearch = "SELECT u.ID FROM USERS u JOIN USERS_IN_GROUPCHAT ug ON u.ID = ug.USER_ID WHERE CHAT_ID = ?;";
    server_.rc = sqlite3_prepare_v2(server_.database, sqlSearch.c_str(), -1, &stmt, nullptr);
    server_.rc = sqlite3_bind_int(stmt, 1, chat_id);
    while ((server_.rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int user_id = sqlite3_column_int(stmt, 0);
        auto it = connectedClientsID.find(user_id);

        if(it != connectedClientsID.end()){
            int userFD = connectedClientsID[user_id];
            std::string message = packMessage(messageId, chat_id ,sender_name, content);
            std::cout << "JSON: " << message << std::endl;

            send(userFD, message.c_str(), message.size(), 0);
        }
        else{
            continue;
        }
    }
}

void Serwer::RequestHandler::handleNewChatRequest(json data, int clientFD){
    std::string chat_name = data.value("chat_name", "");
    int owner_id = connections[clientFD];

    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO GROUPCHATS(chat_name, owner_id) VALUES (?,?);";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    server_.rc = sqlite3_bind_text(stmt, 1, chat_name.c_str(), -1, SQLITE_STATIC );
    server_.rc = sqlite3_bind_int(stmt, 2, owner_id);
    server_.rc = sqlite3_step(stmt);

    if (server_.rc != SQLITE_DONE) {
    std::cerr << "Błąd insertu rekordu: " << sqlite3_errmsg(server_.database) << std::endl;
    }
    long long chat_id = sqlite3_last_insert_rowid(server_.database);
    sqlite3_finalize(stmt);

    
    sql = "INSERT INTO USERS_IN_GROUPCHAT(chat_id, user_id) VALUES (?,?);";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    server_.rc = sqlite3_bind_int(stmt, 1, chat_id);
    server_.rc = sqlite3_bind_int(stmt, 2, owner_id);

    server_.rc = sqlite3_step(stmt);

    if (server_.rc != SQLITE_DONE) {
    std::cerr << "Błąd insertu rekordu: " << sqlite3_errmsg(server_.database) << std::endl;
    }
    sqlite3_finalize(stmt);

    

    addToChat(chat_id, chat_name, clientFD);


}

void Serwer::RequestHandler::addToChat(int chat_id, std::string name, int clientFD){
    std::string mess = packAddToChat(chat_id, name);
    int bytesSent = send(clientFD, mess.c_str(), mess.size(), 0);
}

void Serwer::RequestHandler::handleReadFriendlistRequest(json data, int clientFD){
    int clientID = connections[clientFD];

    sqlite3_stmt* stmt;
    std::string sql = "SELECT USERNAME FROM USERS WHERE ID IN (SELECT USER1_ID FROM FRIENDLIST WHERE USER2_ID = ? UNION SELECT USER2_ID FROM FRIENDLIST WHERE USER1_ID = ?);";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    server_.rc = sqlite3_bind_int(stmt, 1, clientID);
    server_.rc = sqlite3_bind_int(stmt, 2, clientID);

    
    json sendData;
    sendData["request_type"] = "friendlist";
    sendData["data"] = json::array();

    while ((server_.rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        json friendData;
        const unsigned char*  friend_name = sqlite3_column_text(stmt, 0);
        std::string friendNameStr(reinterpret_cast<const char*>(friend_name));

        friendData["username"] = friendNameStr;
        sendData["data"].push_back(friendData);
    }
    send(clientFD, sendData.dump().c_str(), sendData.dump().size(), 0);    

}

void Serwer::RequestHandler::handleAddFriend(json data, int clientFD){
    int clientID = connections[clientFD];
    std::string name = data.value("name", "");

    sqlite3_stmt* stmt;
    std::string sql = "SELECT ID FROM USERS WHERE USERNAME = ?;";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    server_.rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    server_.rc = sqlite3_step(stmt);
    int friendID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    sql = "INSERT INTO FRIENDLIST(USER1_ID, USER2_ID) VALUES(?,?);";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    server_.rc = sqlite3_bind_int(stmt, 1, clientID);
    server_.rc = sqlite3_bind_int(stmt, 2, friendID);

    server_.rc = sqlite3_step(stmt);
    if (server_.rc != SQLITE_DONE) {
    std::cerr << "Błąd insertu rekordu: " << sqlite3_errmsg(server_.database) << std::endl;
    }
    sqlite3_finalize(stmt);

    addFriend(clientID, friendID, name, clientFD);


}

void Serwer::RequestHandler::addFriend(int sender_id, int friend_id, std::string friend_name, int clientFD){
    std::string mess1 = packAddFriend(friend_name);
    send(clientFD, mess1.c_str(), mess1.size(), 0);

    auto it = connectedClientsID.find(friend_id);

    if(it != connectedClientsID.end()){
        int friendFD = connectedClientsID[friend_id];

        sqlite3_stmt* stmt;
        std::string sql = "SELECT USERNAME FROM USERS WHERE ID = ?;";
        server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

        server_.rc = sqlite3_bind_int(stmt, 1, sender_id);

        server_.rc = sqlite3_step(stmt);
        const unsigned char*  sender_name = sqlite3_column_text(stmt, 0);
        std::string senderNameStr(reinterpret_cast<const char*>(sender_name));

        std::string mess2 = packAddFriend(senderNameStr);
        sqlite3_finalize(stmt);

        send(friendFD, mess2.c_str(), mess2.size(), 0);
        
    }
    
}

void Serwer::RequestHandler::handleAddToChat(json data, int clientFD){
    int chat_id = data.value("chat_id", -1);
    std::string name = data.value("name", "");

    sqlite3_stmt* stmt;
    std::string sql = "SELECT ID FROM USERS WHERE USERNAME = ?;";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    server_.rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    server_.rc = sqlite3_step(stmt);
    int user_id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    sql = "INSERT INTO USERS_IN_GROUPCHAT(chat_id, user_id) VALUES (?,?);";

    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    server_.rc = sqlite3_bind_int(stmt, 1, chat_id);
    server_.rc = sqlite3_bind_int(stmt, 2, user_id);
    server_.rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    auto it = connectedClientsID.find(user_id);

    if(it != connectedClientsID.end()){
        int userFD = connectedClientsID[user_id];
        sql = "SELECT CHAT_NAME FROM GROUPCHATS WHERE CHAT_ID = ?";
        server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

        server_.rc = sqlite3_bind_int(stmt, 1, chat_id);
        server_.rc = sqlite3_step(stmt);

        const unsigned char*  chat_name = sqlite3_column_text(stmt, 0);
        std::string chatNameStr(reinterpret_cast<const char*>(chat_name));

        std::string mess = packAddToChat(chat_id, chatNameStr);

        send(userFD, mess.c_str(), mess.size(), 0);

    }


}

