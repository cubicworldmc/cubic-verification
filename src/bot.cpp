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
         Localization& local)
    : src(get_token(token_file)),
      config(std::make_unique<File>(config_file)),
      local(local) {
    src.on_log(dpp::utility::cout_logger());

    src.on_ready([this](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            register_commands();
            register_events();
        }
    });
}

void Bot::register_commands() {
    std::cout << config->get<dpp::snowflake>("guild-id") << std::endl;
    WelcomeCommand::make(src, local, config->get<dpp::snowflake>("guild-id"));
    std::cout << "commands registred" << std::endl;
    std::cout << src.me.id << std::endl;
}

void Bot::register_events() {
    src.on_button_click([this](const dpp::button_click_t& event) {
        if (event.custom_id == "apply-button") {
            event.dialog(Modal::trigger_client(src, local.get("en")));
        }
    });
}

void Bot::run() { src.start(dpp::st_wait); }
