#include "xp_system.h"
#include "xp_system_calculator.h"
#include <dpp/dpp.h>
#include <vector>
#include <algorithm>
#include <random>
#include <string>
#include <format>
#include "db/mongo_database.h"
#include "db/user_entry.h"
#include "rank/rank_util.h"
#include "constants.h"
#include "logging/logging.h"
#include "rank/rank_util.h"
#include "util/owner.h"

const std::vector<dpp::snowflake> ignored_channels = { dpp::snowflake{ 495034452422950915L } };

namespace xp {
    dpp::task<void> give_xp_for_message(const dpp::message_create_t& event) {
        if (std::find(ignored_channels.begin(), ignored_channels.end(), event.msg.channel_id) != ignored_channels.end()) co_return; // Ignore messages in specified channels

        UserEntry user(event.msg.author);

        // XP will only be granted for 7 messages sent within the last 30 seconds.
        if (user.get_recent_message_count() >= 7) co_return;

        int current_xp = user.get_xp();
        int current_level = xp::calculator::level_from_xp(current_xp);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution distribution(2, 5);

        // Award XP.
        int xp_to_give = distribution(gen);
        user.increment_xp(xp_to_give);

        // Increment the rate limit count.
        user.increment_recent_message_count();

        co_await check_for_level_up(current_level, event);
    }

    dpp::task<void> check_for_level_up(const int before_action_level, const dpp::message_create_t& event) {
        const dpp::user& user = event.msg.author;
        UserEntry user_entry(user);
        const int current_level = xp::calculator::level_from_xp(user_entry.get_xp());

        // Check if level up occurred.
        if (current_level <= before_action_level) co_return;

        co_await logging::event(event.owner, "LevelUp", "{} ({}) leveled up to {}: {} XP", user.username, user.id.str(), current_level, user_entry.get_xp());

        const bool is_rank_level = rankutil::is_level_rank_level(current_level);

        if (user_entry.get_level_alerts_preference()) {
            const std::string rank_up_message_text = std::format("Congrats on leveling up! You've reached level **{}** and have received the **{}** rank! :tada:", current_level, rankutil::rank_from_level(current_level).name);
            const std::string level_up_message_text = std::format("Congrats on leveling up! You are now level **{}**! :tada:", current_level);
            const std::string embed_title = is_rank_level ? rank_up_message_text : level_up_message_text;

            dpp::embed embed = dpp::embed{}
                .set_color(HILDA_BLUE)
                .set_title(embed_title)
                .set_description("To disable this message going forward, run `h!levelalert` in this DM or <#495034452422950915>.");

            dpp::message message;
            message.add_embed(embed);

            dpp::confirmation_callback_t callback = co_await event.owner->co_direct_message_create(user.id, message);

            if (callback.is_error()) {
                co_await logging::error(event.owner, "LevelAlert", "Failed to send level up message to {}.\n{}", user.username, callback.get_error().message);
            }
            else {
                co_await logging::event(event.owner, "LevelAlert", "Sent level up message to {}.", user.username);
            }
        }
        else {
            co_await logging::event(event.owner, "LevelAlert", "Not sending a message to {} as they have level alerts off.", user.username);
        }

        if (is_rank_level) {
            const Rank rank = rankutil::rank_from_level(current_level);
            user_entry.increment_tokens(40);

            dpp::guild_member member = event.msg.member;

            member.add_role(rank.role_id);
            dpp::confirmation_callback_t edit_callback = co_await event.owner->co_guild_edit_member(member);

            if (edit_callback.is_error()) {
                dpp::embed embed = dpp::embed{}
                    .set_title("Failed to assign role!")
                    .set_description("Role assignment for a level up did not complete successfully.")
                    .add_field("User", std::format("{} ({})", user.username, user.id.str()), true)
                    .add_field("Role", std::format("{} ({})", rank.name, rank.level), true)
                    .set_color(ERROR_RED);

                co_await logging::error(event.owner, "LevelAlert", "Failed to assign {} to {} ({})!", rank.name, user.username, user.id.str());

                auto owner_id_opt = co_await util::get_owner_id(event.owner);
                if (!owner_id_opt.has_value()) {
                    co_return;
                }

                dpp::message message;
                message.add_embed(embed);

                co_await event.owner->co_direct_message_create(*owner_id_opt, message);
                co_return;
            }
        }
        else {
            user_entry.increment_tokens(20);
        }
    }
}
