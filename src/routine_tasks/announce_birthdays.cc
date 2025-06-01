#include "announce_birthdays.h"
#include <dpp/dpp.h>
#include <vector>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>
#include <chrono>
#include <format>
#include <algorithm>
#include <exception>
#include <string>
#include <optional>
#include "util/helpers.h"
#include "logging/logging.h"
#include "constants.h"
#include "config.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace routine_tasks {
    dpp::task<void> announce_birthdays(dpp::cluster& bot) {
        logging::event(&bot, "Birthdays", "Starting birthdays task.");

        mongocxx::client client(mongocxx::uri(MONGO_URI));
        #ifdef DEBUG
        mongocxx::database db = client["hildabot_test"];
        #else
        mongocxx::database db = client["hildabot"];
        #endif

        std::chrono::zoned_time zt{"America/Chicago", std::chrono::system_clock::now()};
        // get yesterday's date
        auto yesterday = zt.get_local_time() - std::chrono::days(1);
        std::chrono::year_month_day yesterday_ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(yesterday));
        uint32_t yesterday_month = static_cast<uint32_t>(yesterday_ymd.month());
        uint32_t yesterday_day = static_cast<uint32_t>(yesterday_ymd.day());

        // get all users with yesterday's birthday
        auto yesterday_cursor = db["users"].find(
            make_document(kvp("birthday.month", (int) yesterday_month), kvp("birthday.day", (int) yesterday_day))
        );

        for (auto& doc : yesterday_cursor) {
            std::string_view user_id = doc["_id"].get_string();

            std::optional<dpp::guild_member> member_opt = util::get_cached_guild_member(bot, user_id);
            dpp::guild_member member;

            if (!member_opt) {
                auto member_callback = co_await bot.co_guild_get_member(BASE_GUILD_ID, user_id);
                if (member_callback.is_error()) continue; // skip users not in the guild
                member = member_callback.get<dpp::guild_member>();
            }
            else {
                member = member_opt.value();
            }

            const std::vector<dpp::snowflake>& roles = member.get_roles();

            if (std::find(roles.begin(), roles.end(), BIRTHDAY_ROLE_ID) == roles.end()) continue;

            member.remove_role(BIRTHDAY_ROLE_ID);
            co_await bot.co_guild_edit_member(member);
        }

        auto local_time = zt.get_local_time();
        std::chrono::year_month_day ymd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(local_time));
        uint32_t month = static_cast<uint32_t>(ymd.month());
        uint32_t day = static_cast<uint32_t>(ymd.day());

        // get all users with a birthday in the current month and day
        auto cursor = db["users"].find(
            make_document(kvp("birthday.month", (int) month), kvp("birthday.day", (int) day))
        );

        std::vector<dpp::guild_member> birthday_members;

        for (auto& doc : cursor) {
            std::string_view user_id = doc["_id"].get_string();

            std::optional<dpp::guild_member> member_opt = util::get_cached_guild_member(bot, user_id);
            if (member_opt) {
                birthday_members.push_back(member_opt.value());
                continue;
            }

            auto member_callback = co_await bot.co_guild_get_member(BASE_GUILD_ID, user_id);

            if (member_callback.is_error()) continue; // skip users not in the guild

            auto member = member_callback.get<dpp::guild_member>();
            birthday_members.push_back(member);
        }

        if (birthday_members.size() == 1) {
            dpp::message message(std::format("Hey <@{}>! I just wanted to wish you the happiest of birthdays! Can I have a slice of cake too? :birthday: :heart:", birthday_members[0].user_id.str()));
            message.set_channel_id(BIRTHDAY_CHANNEL_ID);

            auto message_callback = co_await bot.co_message_create(message);

            if (message_callback.is_error()) {
                logging::error(&bot, "Birthdays", "Failed to send birthday message: {}", message_callback.get_error().human_readable);
            }
        }
        else if (birthday_members.size() == 2) {
            dpp::message message(std::format("Hey <@{}> and <@{}>! I just wanted to wish you both the happiest of birthdays! Can I have a slice of cake too? :birthday: :heart:", birthday_members[0].user_id.str(), birthday_members[1].user_id.str()));
            message.set_channel_id(BIRTHDAY_CHANNEL_ID);

            auto message_callback = co_await bot.co_message_create(message);

            if (message_callback.is_error()) {
                logging::error(&bot, "Birthdays", "Failed to send birthday message: {}", message_callback.get_error().human_readable);
            }
        }
        else if (birthday_members.size() > 2) {
            std::ostringstream stream;
            stream << "Hey";

            for (auto it = birthday_members.begin(); it != birthday_members.end(); ++it) {
                if (it + 1 != birthday_members.end()) {
                    stream << " <@" << it->user_id.str() << ">,";
                }
                else {
                    stream << " and <@" << it->user_id.str() << ">! ";
                }
            }

            stream << "I just wanted to wish you all the happiest of birthdays! Can I have a slice of cake too? :birthday: :heart:";
            dpp::message message(stream.str());
            message.set_channel_id(BIRTHDAY_CHANNEL_ID);
            auto message_callback = co_await bot.co_message_create(message);
            if (message_callback.is_error()) {
                logging::error(&bot, "Birthdays", "Failed to send birthday message: {}", message_callback.get_error().human_readable);
            }
        }

        for (auto& it : birthday_members) {
            dpp::guild_member member = it;
            member.add_role(BIRTHDAY_ROLE_ID);
            co_await bot.co_guild_edit_member(member);
        }

        logging::event(&bot, "Birthdays", "Completed birthdays task.");
    }
}
