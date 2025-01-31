#pragma once
#ifndef PACKERSSERVER_H
#define PACKERSSERVER_H
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string packLoginSuccess(bool isSuccess);
std::string packMessage(int mess_id, int chat_id, std::string sender_name, std::string content );
std::string packAddToChat(int chat_id, std::string chat_name);
std::string packAddFriend(std::string name);

#endif