#include <dpp/dpp.h>

void give_xp_for_message(const dpp::cluster& bot, const dpp::message_create_t& event);
void check_for_level_up(const int before_action_level, const dpp::user& user, const dpp::guild& server);
