#pragma once

#include <dpp/dpp.h>

#include <fstream>
#include <memory>

#include "command.hpp"
#include "file.hpp"

class Bot {
   public:
    Bot(const std::string& token_file, const std::string& config_file,
        const std::string& en_lang_file);

    void register_commands();
    void run();

   private:
    dpp::cluster src;

    std::unique_ptr<File> config;
    std::unique_ptr<File> en_lang;
};
