#include "streak_warnings.h"
#include <dpp/dpp.h>
#include <string>
#include <format>
#include <bsoncxx/builder/basic/document.hpp>
#include "db/mongo_database.h"
#include "db/user_entry.h"
#include "logging/logging.h"
#include "util/helpers.h"
#include "constants.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace routine_tasks {
    dpp::task<void> streak_warnings(dpp::cluster& bot) {
        co_await logging::event(&bot, "StreakWarnings", "Starting streak warnings task.");

        const int64_t tomorrow_midnight = util::midnight_tomorrow_seconds();
        const int64_t midnight_three_days_from_now = util::midnight_seconds_in_days(3);

        auto tomorrow_cursor = MongoDatabase::get_database()["users"].find(
            make_document(kvp("streak.expiry", tomorrow_midnight))
        );

        for (const auto& doc : tomorrow_cursor) {
            const std::string_view user_id = doc["_id"].get_string();
            UserEntry user_entry(user_id);

            if (!user_entry.get_streak_warnings_preference()) continue;

            dpp::embed embed = dpp::embed()
                .set_title("Your streak expires in one day!")
                .set_color(HILDA_BLUE)
                .set_description(std::format("To disable this warning in the future, run `h!streakwarning` in this DM or <#{}>", BOT_CHANNEL_ID));

            co_await bot.co_direct_message_create(user_id, embed);
        }

        auto three_days_cursor = MongoDatabase::get_database()["users"].find(
            make_document(kvp("streak.expiry", midnight_three_days_from_now))
        );

        for (const auto& doc : three_days_cursor) {
            const std::string_view user_id = doc["_id"].get_string();
            UserEntry user_entry(user_id);

            if (!user_entry.get_streak_warnings_preference()) continue;

            dpp::embed embed = dpp::embed()
                .set_title("Your streak expires in three days!")
                .set_color(HILDA_BLUE)
                .set_description(std::format("To disable this warning in the future, run `h!streakwarning` in this DM or <#{}>", BOT_CHANNEL_ID));

            co_await bot.co_direct_message_create(user_id, embed);
        }

        co_await logging::event(&bot, "StreakWarnings", "Finished streak warnings task.");
    }
}
