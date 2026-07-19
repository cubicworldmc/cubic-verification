#pragma once

#include "file.hpp"

class Localization {
   public:
    Localization();

    void  add(const std::string& lang, const std::string& path);
    File& get(const std::string& lang);

   private:
    std::unordered_map<std::string, File> stash;
};
