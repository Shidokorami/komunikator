#include "../../include/Serwer.h"
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
    sa.sin_port = htons(1100);
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
    std::string sql = "SELECT EXISTS(SELECT 1 FROM USERS WHERE USERNAME = ? AND PASSWORD = ?)";
    server_.rc = sqlite3_prepare_v2(server_.database, sql.c_str(), -1, &stmt, nullptr);

    if (server_.rc!= SQLITE_OK) {
    std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(server_.database) << std::endl;
    return;
    }

    server_.rc = sqlite3_bind_text(stmt, 1, username.c_str(),-1, SQLITE_STATIC);
    server_.rc = sqlite3_bind_text(stmt, 2, password.c_str(),-1, SQLITE_STATIC);

    int exists = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0);  
    }

    std::string mess;
    if (exists == 1) {
        mess = "Zalogowano";
        
    } 
    else 
    {
        mess = "Zle dane";
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

