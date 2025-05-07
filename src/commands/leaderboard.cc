#include "leaderboard.h"
#include <dpp/dpp.h>
#include <vector>
#include <string>
#include <bsoncxx/builder/basic/document.hpp>
#include <format>
#include "db/user_entry.h"
#include "db/mongo_database.h"
#include "constants.h"
#include "util/owner.h"
#include "rank/rank_util.h"
#include "xp/xp_system_calculator.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace commands {
    namespace leaderboard {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if (event.msg.channel_id != BOT_CHANNEL_ID && (co_await util::get_owner_id(event.owner)) != event.msg.author.id) co_return;

            auto cursor = MongoDatabase::get_database()["users"].find({},
                mongocxx::options::find{}
                    .sort(make_document(kvp("xp", -1)))
                    .limit(10)
            );

            dpp::component leaderboard_embed = dpp::component()
                .set_type(dpp::cot_container)
                .set_accent(HILDA_BLUE)
                .add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_text_display)
                        .set_content("## Leaderboard")
                );

            const std::vector<bsoncxx::document::view> leaderboard_entries(cursor.begin(), cursor.end());

            for (std::vector<bsoncxx::document::view>::const_iterator it = leaderboard_entries.begin(); it != leaderboard_entries.end(); ++it) {
                const bsoncxx::document::view& doc = *it;

                std::string_view user_id = doc["_id"].get_string().value;
                const int xp = doc["xp"].get_int32();
                const int level = xp::calculator::level_from_xp(xp);
                const Rank& rank = rankutil::rank_from_level(level);

                auto member_callback = co_await event.owner->co_guild_get_member(BASE_GUILD_ID, user_id);

                if (member_callback.is_error()) continue; // skip users not in the guild

                auto member = member_callback.get<dpp::guild_member>();
                auto user = member.get_user();

                std::string nickname = member.get_nickname();

                leaderboard_embed.add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_text_display)
                        .set_content(std::format("**{}**\n@{}\nLevel {} • {} ({} XP)", nickname.empty() ? user->global_name : nickname, user->username, level, rank.name, xp))
                );

                if (it + 1 != leaderboard_entries.end()) {
                    leaderboard_embed.add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_separator)
                            .set_spacing(dpp::sep_small)
                            .set_divider(true)
                    );
                }
            }

            co_await event.co_send(dpp::message().add_component_v2(leaderboard_embed));
        }
    }
}
