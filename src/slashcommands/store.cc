#include "store.h"
#include <dpp/dpp.h>
#include <vector>
#include <string>
#include <algorithm>
#include <format>
#include <bsoncxx/builder/basic/document.hpp>
#include "db/user_entry.h"
#include "store/store.h"
#include "db/mongo_database.h"
#include "constants.h"
#include "util/helpers.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace slash_commands {
    namespace store {
        dpp::task<void> show_store(const dpp::interaction_create_t& event, const bool is_button) {
            UserEntry user_entry(event.command.usr);

            auto db = MongoDatabase::get_database();

            auto user_doc = db["users"].find_one(
                make_document(kvp("_id", event.command.usr.id.str()))
            ).value();

            auto bson_purchased_items = user_doc["items"]["inventory"].get_array().value;
            std::vector<std::string_view> purchased_items;

            for (const auto& item : bson_purchased_items) {
                purchased_items.push_back(item.get_string().value);
            }

            dpp::component store_embed = dpp::component()
                .set_type(dpp::cot_container)
                .set_accent(HILDA_BLUE)
                .add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_text_display)
                        .set_content("# Store")
                );

            auto user_store_doc = db["store"].find_one(
                make_document(kvp("_id", event.command.usr.id.str()))
            );

            for (const auto& item : store_items::items) {
                if (item->purchase_limit == purchase_limit::ONESHOT && std::find(purchased_items.begin(), purchased_items.end(), item->id) != purchased_items.end()) {
                    continue;
                }

                std::string additional_info_str = item->additional_info;
                bool can_be_purchased = true;

                if (user_store_doc.has_value()) {
                    auto user_store = user_store_doc.value().view();
                    if (user_store.find(item->id) != user_store.end()) {
                        auto item_data = user_store[item->id].get_document().view();
                        int64_t purchasable_again = item_data["purchasable_again"].get_int64();
                        int64_t current_time = util::seconds_since_epoch();

                        if (current_time < purchasable_again) {
                            can_be_purchased = false;
                            additional_info_str = std::format("You can buy this again <t:{}:R>.", purchasable_again);
                        }
                    }
                }

                dpp::component item_component = dpp::component()
                    .set_type(dpp::cot_section)
                    .add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_text_display)
                            .set_content(std::format("### {}", item->name))
                    )
                    .add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_text_display)
                            .set_content(item->description)
                    )
                    .set_accessory(
                        dpp::component()
                            .set_type(dpp::cot_button)
                            .set_label(util::format_with_commas(item->price))
                            .set_emoji("🪙")
                            .set_style(dpp::cos_secondary)
                            .set_disabled(!can_be_purchased || user_entry.get_tokens() < item->price)
                            .set_id(std::format("store_confirm_purchase:{}:{}:{}", event.command.usr.id.str(), item->id, util::seconds_since_epoch()))
                    );

                if (!additional_info_str.empty()) {
                    item_component.add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_text_display)
                            .set_content("-# " + additional_info_str)
                    );
                }

                store_embed.add_component_v2(item_component);
                store_embed.add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_separator)
                        .set_spacing(dpp::sep_small)
                        .set_divider(true)
                );
            }

            store_embed.add_component_v2(
                dpp::component()
                    .set_type(dpp::cot_text_display)
                    .set_content("-# Your token balance: " + util::format_with_commas(user_entry.get_tokens()))
            );

            co_await event.co_reply(is_button ? dpp::ir_update_message : dpp::ir_channel_message_with_source, dpp::message{}.add_component_v2(store_embed));
        }

        dpp::task<void> execute(const dpp::slashcommand_t& event) {
            co_await store::show_store(event, false);
        }

        dpp::task<void> button_execute(const dpp::button_click_t& event) {
            std::istringstream iss(event.custom_id);
            std::vector<std::string> button_data;
            std::string elem;

            while (std::getline(iss, elem, ':')) {
                button_data.push_back(elem);
            }

            const dpp::snowflake user_id = button_data[1];
            const int64_t creation_time = std::stoll(button_data[3]);

            if (event.command.usr.id != user_id) {
                dpp::message reply = dpp::message("You can't use this button. If you wish to use the store, please use the </store:1071965429012107486> command.").set_flags(dpp::m_ephemeral);
                co_await event.co_reply(reply);
                co_return;
            }

            if (creation_time + 300 < util::seconds_since_epoch()) {
                dpp::message reply = dpp::message()
                    .add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_text_display)
                            .set_content("This store session has expired. If you wish to access the store, please use the </store:1071965429012107486> command.")
                    );
                co_await event.co_reply(dpp::ir_update_message, reply);
                co_return;
            }

            co_await store::show_store(event, true);
        }
    }
}
