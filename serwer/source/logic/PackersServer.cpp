#include "PackersServer.h"

std::string  packLoginSuccess(bool isSuccess){

    std::string response = (isSuccess) ? "success" : "failure";
     json data = {
        {"request_response", response},
    };

    return data.dump(4);

}
