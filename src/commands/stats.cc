#include "stats.h"
#include <dpp/dpp.h>
#include <string>
#include <regex>
#include <vector>
#include <cmath>
#include "db/user_entry.h"
#include "constants.h"
#include "xp/xp_system_calculator.h"
#include "rank/rank_util.h"
#include "constants.h"
#include "util/owner.h"

namespace commands {
    namespace stats {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if (event.msg.channel_id != BOT_CHANNEL_ID && (co_await util::get_owner_id(event.owner)) != event.msg.author.id) co_return;

            dpp::snowflake id;
            dpp::user user;

            if (args.empty()) {
                id = event.msg.author.id;
                user = event.msg.author;
            }
            else {
                if (!std::regex_match(args[0], std::regex("^[0-9]{17,19}$"))) {
                    dpp::message reply = dpp::message{}
                        .set_content("Please provide a valid user ID.")
                        .set_channel_id(event.msg.channel_id);

                    co_await event.owner->co_message_create(reply);
                    co_return;
                }

                id = std::stoull(args[0]);
                auto callback = co_await event.owner->co_user_get_cached(id);

                if (callback.is_error()) {
                    // todo: respond with error
                    co_return;
                }

                auto user_identified = callback.get<dpp::user_identified>();
                user = dynamic_cast<dpp::user&>(user_identified);
            }

            dpp::guild_member guild_member;

            if (id != event.msg.author.id) {
                auto member = co_await event.owner->co_guild_get_member(BASE_GUILD_ID, id);

                if (member.is_error()) {
                    dpp::message reply = dpp::message{}
                        .set_content("That ID doesn't seem to belong to anyone here.")
                        .set_channel_id(event.msg.channel_id);

                    co_await event.owner->co_message_create(reply);
                    co_return;
                }

                guild_member = member.get<dpp::guild_member>();
            }
            else {
                guild_member = event.msg.member;
            }

            UserEntry user_entry(user);

            const int current_level = user_entry.get_level();
            const int current_streak = user_entry.get_streak();

            const int xp_for_level_up = xp::calculator::minimum_user_facing_xp_for_level(current_level + 1);
            const int high_score = user_entry.get_high_score();

            const int number_of_filled_in_circles = std::round((float) (user_entry.get_xp() - (float) xp::calculator::minimum_xp_for_level(current_level)) / (float) xp_for_level_up * 20.0f);
            std::stringstream progress_bar;

            for (int i = 0; i < 20; ++i) {
                if (i < number_of_filled_in_circles) {
                    progress_bar << "●";
                } else {
                    progress_bar << "○";
                }
            }

            const std::string current_rank_name = rankutil::rank_from_level(current_level).name;

            dpp::embed embed = dpp::embed{}
                .set_title(guild_member.get_nickname().empty() ? user.global_name : guild_member.get_nickname())
                .set_thumbnail(user.get_avatar_url(1024U, dpp::image_type::i_png, false))
                .set_color(HILDA_BLUE)
                .set_description(std::format("Level {} • {}", current_level, current_rank_name))
                .add_field("Progress", progress_bar.str())
                .add_field("XP", std::format("{}/{}", user_entry.get_xp() - xp::calculator::minimum_xp_for_level(current_level), xp_for_level_up), true)
                .add_field("Streak", std::to_string(current_streak), true)
                .add_field("High Score", std::to_string(high_score), true)
                .add_field("Kudos", std::format("<:HildaStar:539313415425097728> **Given:** {} | <:HildaStar:539313415425097728> **Received:** {}", user_entry.get_kudos_given(), user_entry.get_kudos_received()))
                .add_field("Stats", std::format("**Submits:** {} | **Tokens:** {}", user_entry.get_times_submitted(), user_entry.get_tokens()))
                .add_field("Submit Status", user_entry.has_submitted_today() ? (event.msg.author.id == id ? ":white_check_mark: You have submitted today!" : ":white_check_mark: Submitted today!") : event.msg.author.id == id ? "You have not submitted today." : "Nothing yet today!");

            if (user_entry.get_streak_expiry().has_value()) {
                embed.add_field("Streak Expiry", std::format("<t:{}>", user_entry.get_streak_expiry().value()));
            }

            dpp::message reply = dpp::message{}
                .add_embed(embed)
                .set_channel_id(event.msg.channel_id);

            co_await event.owner->co_message_create(reply);
        }
    }
}
