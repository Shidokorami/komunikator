#pragma once
#ifndef PACKERS_H
#define PACKERS_H
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string packLoginCredentials(std::string username, std::string password);
std::string packRegisterCredentials(std::string username, std::string password);
std::string packReadGroupchatsList();
std::string packReadMessages(int chat_id);
std::string packReadFriendList();
std::string packMessage(int chat_id, std::string content);
std::string packCreateGroup(std::string name);
std::string packAddFriend(std::string name);
std::string packAddToChat(std::string name, int chat_id);

#endif