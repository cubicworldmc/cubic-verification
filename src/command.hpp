#pragma once

#include <dpp/dpp.h>

#include "file.hpp"

class WelcomeCommand {
   public:
    WelcomeCommand();

    static void make(dpp::cluster& src, const std::unique_ptr<File>& local,
                     const dpp::snowflake& guild_id);
};
