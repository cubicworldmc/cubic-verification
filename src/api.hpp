#pragma once

#include <cpr/cpr.h>

#include <cstdint>
#include <iostream>
#include <string>

class API {
   public:
    API(const std::string& host_info_path, const std::string& ca_cert,
        const std::string& client_cert, const std::string& client_key);

    bool accept(const std::string& list, const std::string& code);
    bool decline(const std::string& list, const std::string& code);

   private:
    std::string host;
    size_t      port;

    std::string ca_cert;
    std::string client_cert;
    std::string client_key;

   private:
    struct HostInfo {
        std::string addr;
        size_t      port;
    };

   private:
    bool     verify_code(cpr::Response& response);
    HostInfo get_host_info(const std::string& path);
};
