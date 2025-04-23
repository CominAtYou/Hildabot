#pragma once
#include <dpp/dpp.h>

namespace events {
    dpp::task<void> handle_message_create(const dpp::cluster& bot, const dpp::message_create_t& event);
}
