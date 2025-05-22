#include "store.h"
#include <memory>
#include <string>
#include <format>
#include "util/helpers.h"
#include "constants.h"

dpp::task<void> send_purchase_error_message(const dpp::button_click_t& event, const std::string& error_message) {
    dpp::message reply = dpp::message()
        .add_component_v2(
            dpp::component()
                .set_type(dpp::cot_container)
                .set_accent(ERROR_RED)
                .add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_text_display)
                        .set_content("## Unable to Complete Purchase\n" + error_message)
                )
                .add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_separator)
                        .set_spacing(dpp::sep_small)
                )
                .add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_action_row)
                        .add_component(
                            dpp::component()
                                .set_type(dpp::cot_button)
                                .set_label("OK")
                                .set_id(std::format("store_return_to_menu:{}:{}", event.command.usr.id.str(), util::seconds_since_epoch()))
                                .set_style(dpp::cos_primary)
                        )
                )
        );

    co_await event.co_reply(dpp::ir_update_message, reply);
}

namespace store_items {
    const std::unique_ptr<store_item>& get_store_item(const std::string& id) {
        static const std::unique_ptr<store_item> NOT_FOUND = nullptr;

        for (const auto& item : items) {
            if (item->id == id) {
                return item;
            }
        }
        return NOT_FOUND;
    }
}
