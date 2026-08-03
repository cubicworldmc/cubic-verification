#pragma once

#include <cpr/cpr.h>

#include <cstdint>
#include <iostream>
#include <string>

class API {
   public:
    API(const std::string& host, size_t port);

    bool accept(const std::string& list, const std::string& code);
    bool decline(const std::string& list, const std::string& code);

   private:
    std::string host;
    size_t      port;

   private:
    bool verify_code(cpr::Response& response);
};
