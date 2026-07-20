#pragma once

#include <dpp/dpp.h>

#include "file.hpp"

class Localization {
   public:
    Localization();

    void  add(const std::string& lang, const std::string& path);
    File& get(const std::string& lang);
    File& find(const dpp::guild_member& user);

   private:
    std::unordered_map<std::string, File>           stash;
    std::unordered_map<dpp::snowflake, std::string> langs;
};
