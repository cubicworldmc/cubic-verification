#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

class File {
   public:
    File(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) throw std::runtime_error("failed to load file");

        std::string line = "";
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            size_t pos = line.find("=");
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
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
            std::string value = it->second;
            if constexpr (std::is_same_v<T, std::string>)
                return value;
            else {
                std::istringstream str(value);

                T res{};
                str >> res;
                if (str.fail()) {
                    std::cout << "strign stream failed: " << key << std::endl;
                    return T{};
                }

                return res;
            }
        }

        std::cout << "failed to get info from file: " << key << std::endl;
        return T{};
    }

   private:
    std::unordered_map<std::string, std::string> stash;
};
