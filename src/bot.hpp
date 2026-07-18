#pragma once

#include <dpp/dpp.h>

#include <fstream>
#include <memory>

#include "config.hpp"

class Bot {
   public:
    Bot(const std::string& token_file, const std::string& config_file);

    void register_commands();
    void run();

   private:
    dpp::cluster src;

    std::unique_ptr<Config> config;
};
