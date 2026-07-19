#include <memory>

#include "bot.hpp"
#include "localization.hpp"

#define TOKEN_FILE "resources/token.env"
#define CONFIG_FILE "resources/config.ini"
#define EN_LOCALIZATION_FILE "resources/messages.en"
#define RU_LOCALIZATION_FILE "resources/messages.ru"
#define ES_LOCALIZATION_FILE "resources/messages.es"

int main(void) {
    Localization local;
    local.add("en", EN_LOCALIZATION_FILE);
    local.add("ru", RU_LOCALIZATION_FILE);
    local.add("es", ES_LOCALIZATION_FILE);

    std::unique_ptr<Bot> bot =
        std::make_unique<Bot>(TOKEN_FILE, CONFIG_FILE, local);
    bot->run();

    return 0;
}
