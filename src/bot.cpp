#include "bot.hpp"

static std::string get_token(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("failed to open token file: " + path);

    std::string token;
    std::getline(file, token);

    return token;
}

Bot::Bot(const std::string& token_file, const std::string& config_file)
    : src(get_token(token_file)),
      config(std::make_unique<Config>(config_file)) {
    src.on_log(dpp::utility::cout_logger());
}

void Bot::register_commands() {}

void Bot::run() { src.start(dpp::st_wait); }
