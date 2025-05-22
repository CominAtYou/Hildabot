#include "submit.h"
#include <algorithm>
#include <vector>
#include <regex>
#include <string>
#include <numeric>
#include <format>
#include <cmath>
#include <utility>
#include "db/user_entry.h"
#include "util/helpers.h"
#include "constants.h"
#include "logging/logging.h"
#include "xp/xp_system.h"

namespace commands {
    namespace submit {
        static constexpr uint64_t allowedChannels[] = {
            492578733442465804ULL,
            492579714674720778ULL,
            492580873628286976ULL,
            492580926111481859ULL,
            492885164993675264ULL,
            565327145786802176ULL,
            1139510301390282802ULL,
            1139510635223322674ULL,
            1366529081318838404ULL /* testing channel ID */
        };

        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if (std::find(std::begin(allowedChannels), std::end(allowedChannels), event.msg.channel_id) == std::end(allowedChannels)) {
                co_return;
            }

            UserEntry user(event.msg.author);

            if (user.has_submitted_today()) {
                co_await event.co_reply(std::format("You've already submitted today! You can submit again <t:{}:R>.", util::midnight_tomorrow_seconds()));
                co_return;
            }

            std::stringstream ss;

            for (auto it = args.begin(); it != args.end(); it++) {
                ss << *it;
                if (it + 1 != args.end()) {
                    ss << " ";
                }
            }

            const std::string submission_content = ss.str();

            const std::regex pattern("https?://[-a-zA-Z0-9+&@#/%?=~_|!:,.;]*[-a-zA-Z0-9+&@#/%=~_|].*");
            if (!std::regex_match(submission_content, pattern) && event.msg.attachments.empty()) {
                co_await event.co_reply("You need to provide something to submit!");
                co_return;
            }

            const int current_level = user.get_level();
            int xp_amount = 20 + user.get_streak();

            double multiplier = 1;
            const auto potential_boost = user.shift_out_submit_boost();
            if (potential_boost.has_value()) {
                multiplier = potential_boost.value();
                xp_amount *= multiplier;
            }

            user.increment_xp(xp_amount);
            user.increment_tokens(30);

            const std::pair<int, int64_t> streak_pair = user.process_submission(event.msg.id);
            const int new_streak = streak_pair.first;
            const int64_t streak_expiry = streak_pair.second;

            dpp::embed embed = dpp::embed{}
                .set_title("Submission Successful!")
                .set_author(dpp::embed_author({ event.msg.author.global_name, "", event.msg.author.get_avatar_url(0U, dpp::image_type::i_png, false) }))
                .set_description("Your submission has been accepted!")
                .set_color(SUCCESS_GREEN)
                .add_field("Streak", util::format_with_commas(new_streak), true)
                .add_field("XP Gained", util::format_with_commas(xp_amount) + (multiplier == 1 ? "" : std::format(" ({:.2f}x)", multiplier)), true)
                .add_field("Tokens Received", std::format("30 (Total: {})", util::format_with_commas(user.get_tokens())), true)
                .add_field("Streak Expiry", std::format("<t:{}>", streak_expiry), false)
                .set_footer({ "Tokens can be used with the /store command." });

            co_await event.co_send(dpp::message{}.add_embed(embed));

            logging::event(event.owner, "Submit", "Successfully processed submission from {} ({}) in <#{}>. (Submission ID: {})",
                event.msg.author.username, event.msg.author.id.str(), event.msg.channel_id.str(), event.msg.id.str());

            co_await xp::check_for_level_up(current_level, event.owner, event.msg.author, event.msg.member);
        }
    }
}
