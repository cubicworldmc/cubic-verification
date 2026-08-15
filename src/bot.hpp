#pragma once

#include <dpp/dpp.h>

#include <fstream>
#include <memory>

#include "api.hpp"
#include "command.hpp"
#include "crypto.hpp"
#include "file.hpp"
#include "localization.hpp"
#include "modal.hpp"

#define CWCORE_LIST_NAME "vanilla"

class Bot {
   public:
    Bot(const std::string& token_file, const std::string& config_file,
        Localization& local);

    void register_commands();
    void register_events();
    void run();

   private:
    dpp::cluster src;

    Localization&         local;
    std::unique_ptr<File> config;
    API                   api;

    crypto::ChaCha20_Poly1305 chacha20;
    crypto::AES               aes;

   private:
    std::string to_lower(std::string str);
};
