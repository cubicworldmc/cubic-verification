#include "bot.hpp"

static std::string get_token(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("failed to open token file: " + path);

    std::string token;
    std::getline(file, token);

    return token;
}

Bot::Bot(const std::string& token_file, const std::string& config_file,
         const std::string& en_lang_file)
    : src(get_token(token_file)),
      config(std::make_unique<File>(config_file)),
      en_lang(std::make_unique<File>(en_lang_file)) {
    src.on_log(dpp::utility::cout_logger());

    src.on_ready([this](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            register_commands();
        }
    });
}

void Bot::register_commands() {
    std::cout << config->get<dpp::snowflake>("guild-id") << std::endl;
    WelcomeCommand::make(src, en_lang, config->get<dpp::snowflake>("guild-id"));
    std::cout << "commands registred" << std::endl;
    std::cout << src.me.id << std::endl;
}

void Bot::run() { src.start(dpp::st_wait); }
