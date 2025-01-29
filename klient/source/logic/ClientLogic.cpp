#include "../../include/LoginFrame.h"
#include "../../include/Packers.h"
#include "MainFrame.h"

ClientLogic::ClientLogic(const std::string& serverAddress, int port)
    : serverAddress(serverAddress), port(port), SocketFD(-1) {}

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
    std::cout << mess << std::endl;

    return true;
}

