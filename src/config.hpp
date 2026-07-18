#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

class Config {
   public:
    Config(const std::string& config_file) {
        std::ifstream file(config_file);
        if (!file.is_open())
            throw std::runtime_error("failed to load config file");

        std::string line = "";
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            size_t pos = line.find("=");
            if (pos != std::string::npos) {
                std::string key = line.substr(pos, 0);
                std::string value = line.substr(pos + 1);
                stash[key] = value;
            }
        }
    }

    template <typename T>
    T get(const std::string& key) {
        if (key.empty()) return T{};

        auto it = stash.find(key);
        if (it != stash.end()) {
            std::string        value = it->second;
            std::istringstream str(value);

            T res{};
            str >> res;
            if (str.fail()) {
                std::cout << "string stream failed: " << key << std::endl;
                return T{};
            }

            return res;
        }

        std::cout << "failed to get info from config: " << key << std::endl;
        return T{};
    }

   private:
    std::unordered_map<std::string, std::string> stash;
};
