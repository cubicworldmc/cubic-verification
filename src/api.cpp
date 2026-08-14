#include "api.hpp"

API::API(Crypto& crypto, const std::string& host_info_path)
    : crypto(crypto),
      hinfo(get_host_info(host_info_path)),
      m_socket(-1),
      running(true),
      connected(false) {
    connection_thread = std::thread(&API::conn_loop, this);
}

API::~API() {
    running = false;
    disconnect();

    if (connection_thread.joinable()) connection_thread.join();
}

Response API::accept(const std::string& player, const std::string& list) {
    return send_request("accept", player, list);
}

Response API::decline(const std::string& player, const std::string& list) {
    return send_request("decline", player, list);
}

Response API::query(const std::string& player, const std::string& list) {
    return send_request("query", player, list);
}

void API::conn_loop() {
    while (running) {
        if (!connected) conn_socket();

        std::this_thread::sleep_for(std::chrono::seconds(LOOP_DELAY_TIME));
    }
}

bool API::conn_socket() {
    std::lock_guard lock(socket_mutex);

    if (connected) return true;

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == -1) return false;

    s_addr = {};
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(hinfo.port);
    inet_pton(AF_INET, hinfo.addr.c_str(), &s_addr.sin_addr);

    if (connect(m_socket, reinterpret_cast<sockaddr*>(&s_addr),
                sizeof(s_addr)) < 0) {
        close(m_socket);
        m_socket = -1;
        return false;
    }

    connected = true;
    std::cout << "connected to tcp server" << std::endl;

    return true;
}

void API::disconnect() {
    std::lock_guard lock(socket_mutex);

    if (!connected) return;

    connected = false;
    if (m_socket != -1) {
        shutdown(m_socket, SHUT_RDWR);
        close(m_socket);
        m_socket = -1;
    }

    std::cout << "disconnected from tcp server" << std::endl;
}

Response API::send_request(const std::string& kind, const std::string& player,
                           const std::string& list) {
    std::lock_guard request_lock(request_mutex);

    if (!connected && !conn_socket()) return {0, "", "", "", false};

    const uint64_t timestamp = get_time();

    const std::vector<unsigned char> enc =
        crypto.encrypt(build_msg(timestamp, kind, player, list));
    if (enc.size() < NONCE_LEN + TAG_LEN) return {0, "", "", "", false};

    const uint32_t cipher_size = enc.size() - NONCE_LEN - TAG_LEN;
    const uint32_t net_size = htonl(cipher_size);

    std::vector<unsigned char> packet;
    packet.insert(packet.end(), enc.begin(), enc.begin() + NONCE_LEN);

    const auto* size_ptr = reinterpret_cast<const unsigned char*>(&net_size);
    packet.insert(packet.end(), size_ptr, size_ptr + sizeof(net_size));
    packet.insert(packet.end(), enc.begin() + NONCE_LEN, enc.end());

    {
        std::lock_guard lock(socket_mutex);

        if (!send_all(packet)) {
            disconnect();
            return {0, "", "", "", false};
        }
    }

    std::vector<unsigned char> nonce(NONCE_LEN);
    std::vector<unsigned char> size_buf(sizeof(uint32_t));
    if (!recv_all(nonce, NONCE_LEN) || !recv_all(size_buf, sizeof(uint32_t))) {
        disconnect();
        return {0, "", "", "", false};
    }

    uint32_t net_response_size;
    std::memcpy(&net_response_size, size_buf.data(), sizeof(net_response_size));

    const uint32_t response_size = ntohl(net_response_size);
    if (response_size > 64 * 1024) return {0, "", "", "", false};

    std::vector<unsigned char> cipher_and_tag(response_size + TAG_LEN);
    if (!recv_all(cipher_and_tag, cipher_and_tag.size())) {
        disconnect();
        return {0, "", "", "", false};
    }

    std::vector<unsigned char> encrypted;
    encrypted.insert(encrypted.end(), nonce.begin(), nonce.end());
    encrypted.insert(encrypted.end(), cipher_and_tag.begin(),
                     cipher_and_tag.end());

    std::string response_text;

    try {
        response_text = crypto.decrypt(encrypted);
    } catch (const std::exception& e) {
        std::cerr << "decryption failed: " << e.what() << std::endl;

        disconnect();
        return {0, "", "", "", false};
    }

    Response response = parse_response(response_text);
    if (!response.success) return response;

    if (response.timestamp != timestamp || response.player != player ||
        response.list != list) {
        return {0, "", "", "", false};
    }

    return response;
}

bool API::send_all(const std::vector<unsigned char>& data) {
    size_t sent_total = 0;
    while (sent_total < data.size()) {
        ssize_t sent = send(m_socket, data.data() + sent_total,
                            data.size() - sent_total, 0);

        if (sent <= 0) return false;

        sent_total += static_cast<size_t>(sent);
    }

    return true;
}

bool API::recv_all(std::vector<unsigned char>& buffer, size_t size) {
    size_t read_total = 0;
    while (read_total < size) {
        ssize_t bytes_read =
            recv(m_socket, buffer.data() + read_total, size - read_total, 0);

        if (bytes_read <= 0) return false;

        read_total += static_cast<size_t>(bytes_read);
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

std::string API::build_msg(uint64_t timestamp, const std::string& kind,
                           const std::string& player, const std::string& list) {
    return std::to_string(timestamp) + " " + kind + " " + player + " " + list;
}

Response API::parse_response(const std::string& plaintext) {
    Response resp = {0, "", "", "", false};

    std::stringstream ss(plaintext);
    if (!(ss >> resp.timestamp >> resp.status >> resp.player >> resp.list))
        return resp;

    resp.success = true;

    return resp;
}

uint64_t API::get_time() {
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               now.time_since_epoch())
        .count();
}
