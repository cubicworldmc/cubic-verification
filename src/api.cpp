#include "api.hpp"

API::API(const std::string& host_info_path, const std::string& ca_cert,
         const std::string& client_cert, const std::string& client_key)
    : ca_cert(ca_cert), client_cert(client_cert), client_key(client_key) {
    HostInfo host_info = get_host_info(host_info_path);
    host = host_info.addr;
    port = host_info.port;
}

bool API::accept(const std::string& list, const std::string& code) {
    std::string final_url = "https://" + host + ":" + std::to_string(port) +
                            "/list/" + list + "/" + code + "/accept";

    std::cout << final_url << std::endl;

    auto response =
        cpr::Put(cpr::Url{final_url}, cpr::Ssl(cpr::ssl::CaInfo{ca_cert},
                                               cpr::ssl::CertFile{client_cert},
                                               cpr::ssl::KeyFile{client_key}));

    return verify_code(response);
}

bool API::decline(const std::string& list, const std::string& code) {
    std::string final_url = "https://" + host + ":" + std::to_string(port) +
                            "/list/" + list + "/" + code + "/decline";

    std::cout << final_url << std::endl;

    auto response =
        cpr::Put(cpr::Url{final_url}, cpr::Ssl(cpr::ssl::CaInfo(ca_cert),
                                               cpr::ssl::CertFile{client_cert},
                                               cpr::ssl::KeyFile{client_key}));

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

API::HostInfo API::get_host_info(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("failed to open host info file");

    std::string line;
    std::getline(file, line);

    size_t pos = line.find(":");
    if (pos == std::string::npos)
        throw std::runtime_error("failed to read host info file");

    std::string       addr = line.substr(0, pos);
    std::stringstream ss(line.substr(pos + 1));
    size_t            port = 0;
    ss >> port;

    return HostInfo{addr, port};
}
