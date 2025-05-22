#include "small_xp_increase.h"
#include <dpp/dpp.h>
#include "db/user_entry.h"
#include "xp/xp_system.h"
#include "xp/xp_system_calculator.h"
#include "util/helpers.h"

dpp::task<bool> small_xp_increase::give_item(const dpp::button_click_t& event) const {
    // Get the user entry from the database
    UserEntry user_entry(event.command.usr);

    const int current_xp = user_entry.get_xp();
    const int current_level = xp::calculator::level_from_xp(current_xp);
    user_entry.decrement_tokens(this->price);
    user_entry.increment_xp(150);

    co_await this->send_success_message(event, "You've received an extra 150 XP.");
    co_await xp::check_for_level_up(current_level, event.owner, event.command.usr, event.command.member);
    co_return true;
}
