#include "../../include/Packers.h"

std::string packLoginCredentials(std::string username, std::string password){
     json data = {
        {"request_type", "login"},
        {"data", {
            {"username", username},
            {"password", password}
        }}
    };

    return data.dump(4);

}

std::string packRegisterCredentials(std::string username, std::string password){
     json data = {
        {"request_type", "register"},
        {"data", {
            {"username", username},
            {"password", password}
        }}
    };

    return data.dump(4);

}

std::string packReadGroupchatsList(){
    json data = {
        {"request_type", "read_groupchat_list"},
        {"data", "none"}
    };
    return data.dump(4);
}

std::string packReadMessages(int chat_id){
    json data = {
        {"request_type", "read_messages"},
        {"data", {
        {"chat_id", chat_id}}
    }};
    return data.dump(4);
}

std::string packMessage(int chat_id, std::string content){
    json data = {
        {"request_type", "incoming_message"},
        {"data",{
        {"chat_id", chat_id},
        {"content", content}}
    }};
    return data.dump(4);
}

std::string packCreateGroup(std::string name){
    json data = {
        {"request_type", "create_group"},
        {"data", {
            {"chat_name", name}
        }}
    };

    return data.dump(4);
}

std::string packReadFriendList(){
     json data = {
        {"request_type", "read_friendlist"},
        {"data", "none"}};
    return data.dump(4);
}

std::string packAddFriend(std::string name){
    json data = {
        {"request_type", "add_friend"},
        {"data", {
            {"name", name}
        }}
    };
    return data.dump(4);
}

std::string packAddToChat(std::string name, int chat_id){
    json data = {
        {"request_type", "add_to_chat"},
        {"data", {
            {"name", name},
            {"chat_id", chat_id}
        }}
    };
    return data.dump(4);
}