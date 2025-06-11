#include "streak_warnings.h"
#include <dpp/dpp.h>
#include <string>
#include <format>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>
#include "db/user_entry.h"
#include "logging/logging.h"
#include "util/helpers.h"
#include "constants.h"
#include "config.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace routine_tasks {
    dpp::task<void> streak_warnings(dpp::cluster& bot) {
        logging::event(&bot, "StreakWarnings", "Starting streak warnings task.");

        mongocxx::client client(mongocxx::uri(MONGO_URI));
        #ifdef DEBUG
        mongocxx::database db = client["hildabot_test"];
        #else
        mongocxx::database db = client["hildabot"];
        #endif

        const int64_t tomorrow_midnight = util::midnight_tomorrow_seconds();
        const int64_t midnight_three_days_from_now = util::midnight_seconds_in_days(3);

        auto tomorrow_cursor = db["users"].find(
            make_document(kvp("streak.expiry", tomorrow_midnight))
        );

        for (const auto& doc : tomorrow_cursor) {
            const std::string_view user_id = doc["_id"].get_string();
            UserEntry user_entry(user_id);

            if (!user_entry.get_streak_warnings_preference()) continue;

            dpp::confirmation_callback_t callback = co_await bot.co_user_get_cached(user_id);
            if (callback.is_error()) {
                logging::error(&bot, "StreakWarnings", std::format("Failed to get user {}: {}", user_id, callback.get_error().message));
                continue;
            }

            dpp::user_identified user = callback.get<dpp::user_identified>();

            dpp::embed embed = dpp::embed()
                .set_title("Your streak expires in one day!")
                .set_color(HILDA_BLUE)
                .set_description(std::format("To disable this warning in the future, run `h!streakwarning` in this DM or <#{}>", BOT_CHANNEL_ID));

            auto result = co_await bot.co_direct_message_create(user_id, embed);

            if (result.is_error()) {
                logging::error(&bot, "StreakWarnings", std::format("Failed to send next-day streak warning to {} ({}): {}", user.username, user_id, result.get_error().message));
            } else {
                logging::event(&bot, "StreakWarnings", std::format("Sent next-day streak warning to user {} ({})", user.username, user_id));
            }
        }

        auto three_days_cursor = db["users"].find(
            make_document(kvp("streak.expiry", midnight_three_days_from_now))
        );

        for (const auto& doc : three_days_cursor) {
            const std::string_view user_id = doc["_id"].get_string();
            UserEntry user_entry(user_id);

            if (!user_entry.get_streak_warnings_preference()) continue;

            dpp::confirmation_callback_t user_opt = co_await bot.co_user_get_cached(user_id);
            if (user_opt.is_error()) {
                logging::event(&bot, "StreakWarnings", std::format("Failed to get user {}: {}", user_id, user_opt.get_error().message));
                continue;
            }

            dpp::user_identified user = user_opt.get<dpp::user_identified>();

            dpp::embed embed = dpp::embed()
                .set_title("Your streak expires in three days!")
                .set_color(HILDA_BLUE)
                .set_description(std::format("To disable this warning in the future, run `h!streakwarning` in this DM or <#{}>", BOT_CHANNEL_ID));

            auto result = co_await bot.co_direct_message_create(user_id, embed);

            if (result.is_error()) {
                logging::event(&bot, "StreakWarnings", std::format("Failed to send three-day streak warning to user {} ({}): {}", user.username, user_id, result.get_error().message));
            } else {
                logging::event(&bot, "StreakWarnings", std::format("Sent three-day streak warning to user {} ({})", user.username, user_id));
            }
        }

        logging::event(&bot, "StreakWarnings", "Finished streak warnings task.");
    }
}
