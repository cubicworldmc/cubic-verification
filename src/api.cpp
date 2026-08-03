#include "api.hpp"

API::API(const std::string& host, size_t port) : host(host), port(port) {}

bool API::accept(const std::string& list, const std::string& code) {
    std::string final_url = "http://" + host + ":" + std::to_string(port) +
                            "/list/" + list + "/" + code + "/accept";

    std::cout << final_url << std::endl;

    auto response = cpr::Put(cpr::Url{final_url});

    return verify_code(response);
}

bool API::decline(const std::string& list, const std::string& code) {
    std::string final_url = "http://" + host + ":" + std::to_string(port) +
                            "/list/" + list + "/" + code + "/decline";

    std::cout << final_url << std::endl;

    auto response = cpr::Put(cpr::Url{final_url});

    return verify_code(response);
}

bool API::verify_code(cpr::Response& response) {
    if (response.error) {
        std::cerr << response.error.message << std::endl;
        return false;
    }

    if (response.status_code != 200) {
        std::cerr << "http code: " << response.status_code << " -> "
                  << response.text << std::endl;
        return false;
    }

    return true;
}
