#include <memory>

#include "bot.hpp"

#define TOKEN_FILE "resources/token.env"
#define CONFIG_FILE "resources/config.ini"
#define EN_LOCALIZATION_FILE "resources/messages.en"

int main(void) {
    std::unique_ptr<Bot> bot =
        std::make_unique<Bot>(TOKEN_FILE, CONFIG_FILE, EN_LOCALIZATION_FILE);
    bot->run();

    return 0;
}
