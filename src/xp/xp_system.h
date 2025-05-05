#pragma once
#include <dpp/dpp.h>

namespace xp {
    /// @brief Give XP to a user for sending a message.
    /// @param event The message create event containing the user and message information.
    dpp::task<void> give_xp_for_message(const dpp::message_create_t& event);
    /// @brief Check if a user has leveled up after an action.
    /// @param before_action_level The user's level before the action that may have caused a level up.
    /// @param event The message create event containing the user and message information.
    dpp::task<void> check_for_level_up(const int before_action_level, dpp::cluster* bot, const dpp::user& user, dpp::guild_member member);
}
