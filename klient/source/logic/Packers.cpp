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