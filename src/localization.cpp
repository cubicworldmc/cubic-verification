#include "localization.hpp"

Localization::Localization() {}

void Localization::add(const std::string& lang, const std::string& path) {
    if (lang.empty() || path.empty()) return;
    File file(path);
    stash.emplace(lang, file);
    langs.emplace(file.get<dpp::snowflake>("role-id"), lang);
}

File& Localization::get(const std::string& lang) {
    auto it = stash.find(lang);
    if (it != stash.end()) return it->second;

    throw std::runtime_error("localization not found: " + lang);
}

File& Localization::find(const dpp::guild_member& user) {
    for (dpp::snowflake role : user.get_roles()) {
        auto it = langs.find(role);
        if (it != langs.end()) {
            return get(it->second);
        }
    }

    return get("en");
}
