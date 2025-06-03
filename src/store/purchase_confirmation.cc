#include <dpp/dpp.h>
#include <string>
#include <memory>
#include <format>
#include <bsoncxx/builder/basic/document.hpp>
#include "util/helpers.h"
#include "store/store.h"
#include "store/store_item.h"
#include "db/user_entry.h"
#include "db/mongo_database.h"
#include "constants.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace store {
    namespace purchase_confirmation {
        dpp::task<void> execute(const dpp::button_click_t& event) {
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

            if (user_entry.get_tokens() < item->price) {
                co_await send_purchase_error_message(event, "You don't have enough tokens to buy this item.");
                co_return;
            }

            const auto user_doc = MongoDatabase::get_database()["users"].find_one(make_document(kvp("_id", event.command.usr.id.str()))).value().view();

            if (item->purchase_limit == purchase_limit::ONESHOT) {
                auto purchased_items = user_doc["items"]["inventory"].get_array().value;
                for (const auto& purchased_item : purchased_items) {
                    if (purchased_item.get_string().value == item->id) {
                        co_await send_purchase_error_message(event, "You can't buy this because you already own it.");
                        co_return;
                    }
                }
            }

            const auto store_opt = MongoDatabase::get_database()["store"].find_one(make_document(kvp("_id", event.command.usr.id.str())));

            if (store_opt.has_value()) {
                auto store_doc = store_opt.value().view();

                if (store_doc.find(item->id) != store_doc.end()) {
                    auto item_data = store_doc[item->id].get_document().view();
                    const int64_t purchasable_again = item_data["purchasable_again"].get_int64();
                    const int64_t current_time = util::seconds_since_epoch();

                    if (current_time < purchasable_again) {
                        co_await send_purchase_error_message(event, std::format("You can't purchase this item yet. You can purchase it again <t:{}:R>.", purchasable_again));
                        co_return;
                    }
                }
            }

            co_await event.co_reply(
                dpp::ir_update_message,
                dpp::message()
                    .add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_container)
                            .set_accent(HILDA_BLUE)
                            .add_component_v2(
                                dpp::component()
                                    .set_type(dpp::cot_text_display)
                                    .set_content("## Buy this item?\n" + std::format("Are you sure you want to buy {} for {} tokens?", item->name, util::format_with_commas(item->price)))
                            )
                            .add_component_v2(
                                dpp::component()
                                    .set_type(dpp::cot_separator)
                                    .set_spacing(dpp::sep_small)
                            )
                            .add_component_v2(
                                dpp::component()
                                    .set_type(dpp::cot_action_row)
                                    .add_component_v2(
                                        dpp::component()
                                            .set_type(dpp::cot_button)
                                            .set_label("Yes")
                                            .set_id(std::format("store_execute_purchase:{}:{}:{}", event.command.usr.id.str(), item->id, util::seconds_since_epoch()))
                                            .set_style(dpp::cos_primary)
                                    )
                                    .add_component_v2(
                                        dpp::component()
                                            .set_type(dpp::cot_button)
                                            .set_label("No")
                                            .set_id(std::format("store_return_to_menu:{}:{}", event.command.usr.id.str(), util::seconds_since_epoch()))
                                            .set_style(dpp::cos_secondary)
                                    )
                            )
                    )
            );
        }
    }
}
