#pragma once
#ifndef PACKERSSERVER_H
#define PACKERSSERVER_H
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string packLoginSuccess(bool isSuccess);

#endif