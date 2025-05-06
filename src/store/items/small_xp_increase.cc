#include "small_xp_increase.h"
#include <dpp/dpp.h>
#include "db/user_entry.h"
#include "constants.h"
#include "xp/xp_system.h"
#include "xp/xp_system_calculator.h"
#include "util/helpers.h"

dpp::task<void> small_xp_increase::give_item(const dpp::button_click_t& event) const {
    // Get the user entry from the database
    UserEntry user_entry(event.command.usr);

    const int current_xp = user_entry.get_xp();
    const int current_level = xp::calculator::level_from_xp(current_xp);
    user_entry.decrement_tokens(this->price);
    user_entry.increment_xp(50);

    dpp::message message = dpp::message()
        .add_component_v2(
            dpp::component()
                .set_type(dpp::cot_container)
                .set_accent(SUCCESS_GREEN)
                .add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_text_display)
                        .set_content("## Purchase Successful!\nYou've received an extra 50 XP.")
                )
                .add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_text_display)
                        .set_content("-# Token Balance: " + util::format_with_commas(user_entry.get_tokens()))
                )
        );

    co_await event.co_reply(dpp::ir_update_message, message);
    co_await xp::check_for_level_up(current_level, event.owner, event.command.usr, event.command.member);
}
