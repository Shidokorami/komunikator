#include "../../include/LoginFrame.h"
#include "../../include/Packers.h"
#include "MainFrame.h"

ClientLogic::ClientLogic(const std::string& serverAddress, int port)
    : serverAddress(serverAddress), port(port), SocketFD(-1) {
        rc = sqlite3_open("./resources/resource/klientDB.db", &database);

        if (rc != SQLITE_OK) {
        std::cerr << "Błąd otwierania bazy danych: " << sqlite3_errmsg(database) << std::endl;

    }
    }

bool ClientLogic::connectToServer(){
    SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD == -1)
    {
        std::cerr << "Client Socket Error" << std::endl;
        return false;
    }
    

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(serverAddress.c_str());
    server_addr.sin_port = htons(port);

    if (connect(SocketFD, (struct sockaddr*)& server_addr, sizeof server_addr) == -1){
        std::cerr << "Connection Error" << std::endl;
        return false;
    }

    return true;

}

bool ClientLogic::Login(std::string username, std::string password){
    std::string credentials = packLoginCredentials(username, password);

    send(SocketFD, credentials.c_str(), credentials.size(), 0);
    char buff[256];
    
    ssize_t bytesRecv = recv(SocketFD, buff, sizeof buff, 0);
    buff[bytesRecv] = '\0';

    json request = json::parse(buff);
    std::string result = request["request_response"];
    std::cout << result << std::endl;

    if(result == "success"){
        return true;
    }
    else{
        return false;
    }

}

bool ClientLogic::Register(std::string username, std::string password){
    std::string credentials = packRegisterCredentials(username, password);

    send(SocketFD, credentials.c_str(), credentials.size(), 0);
    char buff[256];
    
    ssize_t bytesRecv = recv(SocketFD, buff, sizeof buff, 0);
    buff[bytesRecv] = '\0';

    std::string mess(buff);

    return true;
}

void ClientLogic::getGroupchatsList(){
    std::string request = packReadGroupchatsList();
    char buffer[MAX_BUFF_SIZE];

    send(SocketFD, request.c_str(), request.size(), 0);


    int bytesRecv = recv(SocketFD, buffer, MAX_BUFF_SIZE -1, 0);
     if (bytesRecv <= 0) {
        std::cerr << "Błąd przy odbieraniu danych lub połączenie zamknięte!" << std::endl;
        return;
    }

    buffer[bytesRecv] = '\0';
    std::string dataString(buffer);

    json data = json::parse(dataString);
    
    for (const auto& chat : data["data"]) {
        int chat_id = chat.value("chat_id", -1);
        std::string chat_name = chat.value("chat_name", "");
        sendGroupchatsToDB(chat_id, chat_name);

    }

}

void ClientLogic::sendGroupchatsToDB(int chat_id, std::string chat_name){
    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO GROUPCHATS(chat_id, name) VALUES (?, ?);";
    int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(database) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_text(stmt, 2, chat_name.c_str(), -1, SQLITE_STATIC);
     if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Błąd przy zapisie czatów: " << sqlite3_errmsg(database) << std::endl;
    } else {
        std::cout << "Czat zapisany do bazy!\n";
    }
    sqlite3_finalize(stmt);
}

void ClientLogic::getMessages(){
    sqlite3_stmt* stmt;
    std::string sql = "SELECT CHAT_ID FROM GROUPCHATS";
    int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(database) << std::endl;
        return;
    }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {  
        int chat_id = sqlite3_column_int(stmt, 0);
        
        char buffer[MAX_BUFF_SIZE];
        std::string request = packReadMessages(chat_id);

        send(SocketFD, request.c_str(), request.size(), 0);
        
        int bytesRecv = recv(SocketFD, buffer, MAX_BUFF_SIZE - 1, 0);
        buffer[bytesRecv] = '\0';  
        std::string serverResponse(buffer);
        std::cout << serverResponse << std::endl;
        json messages = json::parse(serverResponse);

        for (const auto& mess: messages["data"]) {
            
            int rcInsert;
            int mess_id = mess.value("message_id", -1);
            std::string sender = mess.value("sender", "");
            std::string content = mess.value("content", "");

            std::string insertSql = "INSERT INTO MESSAGES(MESS_ID, CHAT_ID, CONTENT, SENDER_NAME) VALUES(?,?,?,?)";
            sqlite3_stmt* insertStmt;

            rcInsert=sqlite3_prepare_v2(database, insertSql.c_str(), -1, &insertStmt, nullptr);
            if (rcInsert != SQLITE_OK) {
            std::cerr << "Błąd przygotowania do insertu wiadomosci: " << sqlite3_errmsg(database) << std::endl;
            continue; 
            }
            sqlite3_bind_int(insertStmt, 1, mess_id);
            sqlite3_bind_int(insertStmt, 2, chat_id);
            sqlite3_bind_text(insertStmt, 3, content.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(insertStmt, 4, sender.c_str(), -1, SQLITE_STATIC);

            rcInsert = sqlite3_step(insertStmt);

            if (rcInsert != SQLITE_DONE) {
            std::cerr << "Błąd insertu rekordu: " << sqlite3_errmsg(database) << std::endl;
            }
            sqlite3_finalize(insertStmt);
        }   
    }

    
}

void ClientLogic::sendMessage(int chat_id, std::string content){
    std::string message = packMessage(chat_id, content);

    send(SocketFD, message.c_str(), message.size(), 0);
}

std::string ClientLogic::messageListener(){
    char buff[MAX_BUFF_SIZE];
    int bytesRecv = recv(SocketFD, buff, MAX_BUFF_SIZE -1, 0);
    buff[bytesRecv] = '\0';
    std::cout << "Bufor: " << buff << std::endl;
    std::string mess(buff);
    return mess;
    
}

void ClientLogic::createNewChat(std::string name){
    std::string message = packCreateGroup(name);
    send(SocketFD, message.c_str(), message.size(), 0);
}

void ClientLogic::getFriendList(){
    std::string request = packReadFriendList();
     char buffer[MAX_BUFF_SIZE];

    send(SocketFD, request.c_str(), request.size(), 0);


    int bytesRecv = recv(SocketFD, buffer, MAX_BUFF_SIZE -1, 0);
     if (bytesRecv <= 0) {
        std::cerr << "Błąd przy odbieraniu danych lub połączenie zamknięte!" << std::endl;
        return;
    }

    buffer[bytesRecv] = '\0';
    std::string dataString(buffer);

    json data = json::parse(dataString);

    for (const auto& friend_user : data["data"]) {
        std::string name = friend_user.value("username", "");
        sendFriendToDB(name);

    }
}

void ClientLogic::sendFriendToDB(std::string name){
    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO FRIENDS(USERNAME) VALUES(?);";
    int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(database) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Błąd przy zapisie znajomych: " << sqlite3_errmsg(database) << std::endl;
    } else {
        std::cout << "znajomy zapisany do bazy!\n";
    }
    sqlite3_finalize(stmt);
}

void ClientLogic::addFriend(std::string name){
    std::string message = packAddFriend(name);
    send(SocketFD, message.c_str(), message.size(), 0);
}

void ClientLogic::addToChat(int chat_id, std::string name){
    std::string mess = packAddToChat(name, chat_id);
    send(SocketFD, mess.c_str(), mess.size(), 0);
}





