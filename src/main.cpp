#include <memory>

#include "bot.hpp"

#define TOKEN_FILE "resources/.env"
#define CONFIG_FILE "resources/config.ini"

int main(void) {
    std::unique_ptr<Bot> bot = std::make_unique<Bot>(TOKEN_FILE, CONFIG_FILE);

    bot->register_commands();
    bot->run();

    return 0;
}
