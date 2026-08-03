#pragma once

#include <dpp/dpp.h>

#include <fstream>
#include <memory>

#include "api.hpp"
#include "command.hpp"
#include "file.hpp"
#include "localization.hpp"
#include "modal.hpp"

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
};
