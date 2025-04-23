#pragma once
#include <dpp/dpp.h>

namespace xp {
    dpp::task<void> give_xp_for_message(const dpp::message_create_t& event);
    dpp::task<void> check_for_level_up(const int before_action_level, const dpp::message_create_t& event);
}
