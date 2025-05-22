#include "medium_xp_increase.h"
#include <dpp/dpp.h>
#include "db/user_entry.h"
#include "xp/xp_system_calculator.h"
#include "xp/xp_system.h"

dpp::task<bool> medium_xp_increase::give_item(const dpp::button_click_t& event) const {
    UserEntry user_entry(event.command.usr);
    const int current_level = xp::calculator::level_from_xp(user_entry.get_xp());

    user_entry.increment_xp(800);
    user_entry.decrement_tokens(this->price);

    co_await this->send_success_message(event, "You've received an extra 800 XP.");
    co_await xp::check_for_level_up(current_level, event.owner, event.command.usr, event.command.member);
    co_return true;
}
