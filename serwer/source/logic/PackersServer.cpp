#include "PackersServer.h"

std::string  packLoginSuccess(bool isSuccess){

    std::string response = (isSuccess) ? "success" : "failure";
     json data = {
        {"request_response", response},
    };

    return data.dump(4);
}

std::string packMessage(int mess_id, int chat_id, std::string sender_name, std::string content ){
    json data = {
        {"request", "incoming_message"},
        {"data",{
        {"mess_id", mess_id},
        {"chat_id", chat_id},
        {"sender_name", sender_name},
        {"content", content} }
    }};
    return data.dump(4);
}

