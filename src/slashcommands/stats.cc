#include "stats.h"
#include <dpp/dpp.h>
#include "db/user_entry.h"
#include "xp/xp_system_calculator.h"
#include "rank/rank_util.h"
#include "constants.h"
#include "util/helpers.h"

namespace slash_commands {
    namespace stats {
        dpp::task<void> execute(const dpp::slashcommand_t& event) {
            dpp::command_interaction command = event.command.get_command_interaction();

            dpp::guild_member guild_member = event.command.member;
            if (!command.options.empty()) {
                guild_member = event.command.get_resolved_member(command.get_value<dpp::snowflake>(0));
            }

            UserEntry user_entry(guild_member.user_id);

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
                .set_title(guild_member.get_nickname().empty() ? guild_member.get_user()->global_name : guild_member.get_nickname())
                .set_thumbnail(guild_member.get_user()->get_avatar_url(1024U, dpp::image_type::i_png, false))
                .set_color(HILDA_BLUE)
                .set_description(std::format("Level {} • {}", current_level, current_rank_name))
                .add_field("Progress", progress_bar.str())
                .add_field("XP", std::format("{}/{}", util::format_with_commas(user_entry.get_xp() - xp::calculator::minimum_xp_for_level(current_level)), util::format_with_commas(xp_for_level_up)), true)
                .add_field("Streak", util::format_with_commas(current_streak), true)
                .add_field("High Score", util::format_with_commas(high_score), true)
                .add_field("Kudos", std::format("<:HildaStar:539313415425097728> **Given:** {} | <:HildaStar:539313415425097728> **Received:** {}", util::format_with_commas(user_entry.get_kudos_given()), util::format_with_commas(user_entry.get_kudos_received())))
                .add_field("Stats", std::format("**Submits:** {} | **Tokens:** {}", util::format_with_commas(user_entry.get_times_submitted()), util::format_with_commas(user_entry.get_tokens())))
                .add_field("Submit Status", user_entry.has_submitted_today() ? (event.command.usr.id == guild_member.user_id ? ":white_check_mark: You have submitted today!" : ":white_check_mark: Submitted today!") : event.command.usr.id == guild_member.user_id ? "You have not submitted today." : "Nothing yet today!");

            if (user_entry.get_streak_expiry().has_value()) {
                embed.add_field("Streak Expiry", std::format("<t:{}>", user_entry.get_streak_expiry().value()));
            }

            co_await event.co_reply(dpp::message{}.add_embed(embed));
        }
    }
}
