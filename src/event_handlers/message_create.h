#pragma once
#include <dpp/dpp.h>

namespace events {
    void handle_message_create(const dpp::cluster& bot, const dpp::message_create_t& event);
}
