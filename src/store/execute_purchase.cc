#include "execute_purchase.h"
#include <dpp/dpp.h>
#include "util/helpers.h"
#include "db/user_entry.h"
#include "store/store.h"
#include "logging/logging.h"

namespace store {
    dpp::task<void> execute_purchase(const dpp::button_click_t& event) {
        // structure: command:userid:itemid
        std::istringstream iss(event.custom_id);
        std::vector<std::string> button_data;
        std::string elem;

        while (std::getline(iss, elem, ':')) {
            button_data.push_back(elem);
        }

        dpp::snowflake user_id = button_data[1];
        std::string item_id = button_data[2];
        int64_t created_at = std::stoll(button_data[3]);

        if (event.command.usr.id != user_id) {
            dpp::message reply = dpp::message("You can't use this button. If you wish to use the store, please use the </store:1071965429012107486> command.").set_flags(dpp::m_ephemeral);
            co_await event.co_reply(reply);
            co_return;
        }

        if (created_at + 300 < util::seconds_since_epoch()) {
            dpp::message reply = dpp::message()
                .add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_text_display)
                        .set_content("This store session has expired. If you wish to access the store, please use the </store:1071965429012107486> command.")
                );
            co_await event.co_reply(dpp::ir_update_message, reply);
            co_return;
        }

        UserEntry user_entry(event.command.usr);

        const auto& item = store_items::get_store_item(item_id);
        if (item == nullptr) {
            // whar?
            co_return;
        }

        co_await item->give_item(event);
        co_await logging::event(event.owner, "Store", "{} ({}) purchased {} for {} tokens.", event.command.usr.username, event.command.usr.id.str(), item->name, item->price);
    }
}
