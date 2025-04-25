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
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if (std::find(std::begin(allowedChannels), std::end(allowedChannels), event.msg.channel_id) == std::end(allowedChannels)) {
                co_return;
            }

            UserEntry user(event.msg.author);

            if (user.has_submitted_today()) {
                dpp::message reply = dpp::message{}
                    .set_content(std::format("You've already submitted today! You can submit again <t:{}:R>.", util::midnight_tomorrow_seconds()))
                    .set_channel_id(event.msg.channel_id);

                co_await event.owner->co_message_create(reply);
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
                dpp::message reply = dpp::message{}
                    .set_content("You need to provide something to submit!")
                    .set_reference(event.msg.id)
                    .set_allowed_mentions()
                    .set_channel_id(event.msg.channel_id);

                co_await event.owner->co_message_create(reply);
                co_return;
            }

            int current_level = user.get_level();
            int xp_amount = 20 + static_cast<int>(std::floor(0.5f * user.get_streak()));

            double multiplier = 1;
            const auto potential_boost = user.shift_out_submit_boost();
            if (potential_boost.has_value()) {
                multiplier = potential_boost.value();
                xp_amount *= multiplier;
            }

            user.increment_xp(xp_amount);
            user.set_latest_submission_id(event.msg.id);
            user.mark_submitted_today();
            user.increment_times_submitted();
            user.increment_tokens(30);

            const std::pair<int, int64_t> streak_pair = user.increment_streak();
            const int new_streak = streak_pair.first;
            const int64_t streak_expiry = streak_pair.second;

            dpp::embed embed = dpp::embed{}
                .set_title("Submission Successful!")
                .set_author(dpp::embed_author({ event.msg.author.global_name, "", event.msg.author.get_avatar_url(0U, dpp::image_type::i_png, false) }))
                .set_description("Your submission has been accepted!")
                .set_color(SUCCESS_GREEN)
                .add_field("Streak", std::to_string(new_streak), true)
                .add_field("XP Gained", std::to_string(xp_amount) + (multiplier == 1 ? "" : std::format(" ({:.2f}x)", multiplier)), true)
                .add_field("Tokens Received", std::format("30 (Total: {})", user.get_tokens()), true)
                .add_field("Streak Expiry", std::format("<t:{}>", streak_expiry), false)
                .set_footer({ "Tokens can be used with the /store command." });

            co_await event.owner->co_message_create(dpp::message{}.add_embed(embed).set_channel_id(event.msg.channel_id));

            co_await logging::event(event.owner, "Submit", "Successfully processed submission from {} ({}) in <#{}>. (Submission ID: {})",
                event.msg.author.username, event.msg.author.id.str(), event.msg.channel_id.str(), event.msg.id.str());

            co_await xp::check_for_level_up(current_level, event);
        }
    }
}
