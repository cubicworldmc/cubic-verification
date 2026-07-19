#pragma once

#include <dpp/dpp.h>

#include "localization.hpp"

class WelcomeCommand {
   public:
    WelcomeCommand();

    static void make(dpp::cluster& src, Localization& local,
                     const dpp::snowflake& guild_id);
};
