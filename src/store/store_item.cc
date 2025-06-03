#include "store_item.h"
#include <dpp/dpp.h>
#include "constants.h"
#include "db/user_entry.h"
#include "util/helpers.h"

dpp::task<void> store_item::send_success_message(const dpp::button_click_t& event, const std::string& message) const {
    UserEntry user_entry(event.command.usr);

    co_await event.co_reply(
        dpp::ir_update_message,
        dpp::message()
            .add_component_v2(
                dpp::component()
                    .set_type(dpp::cot_container)
                    .set_accent(SUCCESS_GREEN)
                    .add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_text_display)
                            .set_content("## Purchase Successful!\n" + message)
                    )
                    .add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_text_display)
                            .set_content("-# Token Balance: " + util::format_with_commas(user_entry.get_tokens()))
                    )
            )
            .add_component_v2(
                dpp::component()
                    .set_type(dpp::cot_action_row)
                    .add_component(
                        dpp::component()
                            .set_type(dpp::cot_button)
                            .set_label("Return to Store")
                            .set_id(std::format("store_return_to_menu:{}:{}", event.command.usr.id.str(), util::seconds_since_epoch()))
                            .set_style(dpp::cos_primary)
                    )
            )
    );
}
