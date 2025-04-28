#include <dpp/dpp.h>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include "constants.h"
#include "db/user_entry.h"
#include "rank/rank_util.h"
#include "logging/logging.h"

namespace commands {
    namespace levelcheck {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if (event.msg.channel_id != BOT_CHANNEL_ID && event.msg.channel_id != TESTING_CHANNEL_ID) {
                co_return;
            }

            UserEntry user_entry(event.msg.author);
            const dpp::user user = event.msg.author;

            if (user_entry.get_xp() < 290) {
                dpp::message reply = dpp::message{}
                    .set_content("You don't seem to be missing any roles, so you're all set!")
                    .set_reference(event.msg.id)
                    .set_channel_id(event.msg.channel_id)
                    .set_allowed_mentions();

                co_await event.owner->co_message_create(reply);
                co_return;
            }

            const int rank_level = rankutil::rank_from_level(user_entry.get_level()).level;
            const int index = std::distance(rankutil::rank_levels, std::find(rankutil::rank_levels, rankutil::rank_levels + sizeof(rankutil::rank_levels) / sizeof(rankutil::rank_levels[0]), rank_level));

            dpp::guild_member member = event.msg.member;
            const std::vector<dpp::snowflake>& roles = member.get_roles();

            int restored_roles = 0;
            for (int i = index; i >= 0; i--) {
                const int level = rankutil::rank_levels[i];
                const int64_t role_id = rankutil::rank_from_level(level).role_id;
                dpp::snowflake role = dpp::snowflake(role_id);

                if (std::find(roles.begin(), roles.end(), role) == roles.end()) {
                    member.add_role(role);
                    restored_roles++;
                }
            }

            if (restored_roles > 0) {
                co_await event.owner->co_guild_edit_member(member);

                dpp::message reply = dpp::message{}
                    .set_content("You're all set. The roles you were missing have been added to you.")
                    .set_reference(event.msg.id)
                    .set_channel_id(event.msg.channel_id)
                    .set_allowed_mentions();

                co_await event.owner->co_message_create(reply);
                co_await logging::event(event.owner, "LevelCheck", "Restored {} role(s) for {} ({})", restored_roles, user.username, user.id.str());
            }
            else {
                dpp::message reply = dpp::message{}
                    .set_content("You don't seem to be missing any roles, so you're all set!")
                    .set_reference(event.msg.id)
                    .set_channel_id(event.msg.channel_id)
                    .set_allowed_mentions();

                co_await event.owner->co_message_create(reply);
            }
        }
    }
}
