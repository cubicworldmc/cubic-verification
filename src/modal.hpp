#pragma once

#include <dpp/dpp.h>

#include "localization.hpp"

class Modal {
   public:
    static dpp::interaction_modal_response trigger_client(
        const dpp::cluster& src, File& local);
    static dpp::interaction_modal_response trigger_reject(
        const dpp::cluster& src, const std::string user_id);
};
