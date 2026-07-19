#include "localization.hpp"

Localization::Localization() {}

void Localization::add(const std::string& lang, const std::string& path) {
    if (lang.empty() || path.empty()) return;
    stash.emplace(lang, File(path));
}

File& Localization::get(const std::string& lang) {
    auto it = stash.find(lang);
    if (it != stash.end()) return it->second;

    throw std::runtime_error("localization not found: " + lang);
}
