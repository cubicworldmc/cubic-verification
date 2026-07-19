#pragma once

#include <dpp/dpp.h>

#include "localization.hpp"

class Modal {
   public:
    static dpp::interaction_modal_response trigger_client(
        const dpp::cluster& src, File& local);
    void trigger_mod(const dpp::cluster& src, File& local);
};
