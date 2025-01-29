#pragma once
#ifndef PACKERS_H
#define PACKERS_H
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string packLoginCredentials(std::string username, std::string password);
std::string packRegisterCredentials(std::string username, std::string password);

#endif