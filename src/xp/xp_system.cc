#include "xp_system.h"
#include "xp_system_calculator.h"
#include <dpp/dpp.h>
#include <vector>
#include <algorithm>
#include <random>
#include "db/mongo_database.h"
#include "db/user_entry.h"

const std::vector<dpp::snowflake> ignored_channels = { dpp::snowflake{ 495034452422950915L } };
const std::mt19937 gen(std::random_device());
std::uniform_int_distribution<> distribution(2, 6);

void give_xp_for_message(const dpp::cluster& bot, const dpp::message_create_t& event) {
    if (std::find(ignored_channels.begin(), ignored_channels.end(), event.msg.channel_id) != ignored_channels.end()) return; // Ignore messages in specified channels

    UserEntry user(event.msg.author);

    if (user.get_recent_message_count() >= 7) {
        return;
    }

    int current_xp = user.get_xp();
    int current_level = level_from_xp(current_xp);
    int xp_to_give = distribution(gen);

    user.increment_xp(xp_to_give);
    user.increment_recent_message_count();
}

void check_for_level_up(const int before_action_level, const dpp::user& user, const dpp::guild& server) {
    UserEntry user_entry(user);
    int current_level = level_from_xp(user_entry.get_xp());
}
