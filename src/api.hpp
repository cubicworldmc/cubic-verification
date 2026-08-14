#pragma once

#include <arpa/inet.h>
#include <endian.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "crypto.hpp"

#define LOOP_DELAY_TIME 10

struct Response {
    uint64_t    timestamp;
    std::string status;
    std::string player;
    std::string list;
    std::string error;
    bool        success;
};

class API {
   public:
    API(crypto::ChaCha20_Poly1305& crypto, const std::string& host_info_path);
    ~API();

    Response accept(const std::string& player, const std::string& list);
    Response decline(const std::string& player, const std::string& list);
    Response query(const std::string& player, const std::string& list);

   private:
    struct HostInfo {
        std::string addr;
        size_t      port;
    };

   private:
    crypto::ChaCha20_Poly1305& crypto;

    HostInfo    hinfo;
    int         m_socket;
    sockaddr_in s_addr;

    std::atomic<bool> running;
    std::atomic<bool> connected;
    std::mutex        socket_mutex;
    std::mutex        request_mutex;

    std::thread connection_thread;

   private:
    void conn_loop();
    bool conn_socket();
    void disconnect();

    Response send_request(const std::string& kind, const std::string& player,
                          const std::string& list);

    bool send_all(const std::vector<unsigned char>& data);
    bool recv_all(std::vector<unsigned char>& buffer, size_t size);

    HostInfo    get_host_info(const std::string& path);
    std::string build_msg(uint64_t timestamp, const std::string& kind,
                          const std::string& player, const std::string& list);
    Response    parse_response(const std::string& plaintext);
    uint64_t    get_time();
};
