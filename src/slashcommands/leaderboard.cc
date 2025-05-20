#include "leaderboard.h"
#include <dpp/dpp.h>
#include <vector>
#include <string>
#include <bsoncxx/builder/basic/document.hpp>
#include <format>
#include "db/user_entry.h"
#include "db/mongo_database.h"
#include "util/helpers.h"
#include "constants.h"
#include "util/owner.h"
#include "rank/rank_util.h"
#include "xp/xp_system_calculator.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

struct leaderboard_entry {
    dpp::guild_member member;
    int xp;
    int pos;
};

namespace slash_commands {
    namespace leaderboard {
        dpp::task<void> execute(const dpp::slashcommand_t& event) {
            auto count = MongoDatabase::get_database()["users"].estimated_document_count();
            auto cursor = MongoDatabase::get_database()["users"].find(
                {},
                mongocxx::options::find().sort(
                    make_document(
                        kvp("xp", -1)
                    )
                )
            );

            std::vector<leaderboard_entry> leaderboard_entries;
            leaderboard_entries.reserve(count);
            dpp::members_container members = event.command.get_guild().members;

            int pos = 1;
            for (const auto& doc : cursor) {
                const dpp::snowflake user_id = doc["_id"].get_string().value;
                if (members.find(user_id) == members.end()) continue; // skip users not in the guild
                leaderboard_entries.emplace_back(members[user_id], doc["xp"].get_int32().value, pos++);
            }

            auto interac = event.command.get_command_interaction();
            size_t start_idx = 0;

            if (leaderboard_entries.size() == 0) {
                co_await event.co_reply("Something went wrong while trying to get the leaderboard.");
                co_return;
            }

            if (!interac.options.empty() && interac.get_value<bool>(0)) {
                auto it = std::find_if(
                    leaderboard_entries.begin(),
                    leaderboard_entries.end(),
                    [&](auto& entry) {
                        return entry.member.user_id == event.command.usr.id;
                    }
                );
                if (it != leaderboard_entries.end() && leaderboard_entries.size() > 5) {
                    size_t idx = std::distance(leaderboard_entries.begin(), it);
                    // aim to put the user at position 2 of the 5‐item window
                    size_t raw = (idx < 2 ? 0 : idx - 2);
                    // clamp so window doesn’t run off the end
                    start_idx = std::min(raw, leaderboard_entries.size() - 5);
                }
            }

            dpp::component leaderboard_embed = dpp::component()
                .set_type(dpp::cot_container)
                .set_accent(HILDA_BLUE)
                .add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_text_display)
                        .set_content("## Leaderboard")
                );

            size_t end_idx = std::min(start_idx + 5, leaderboard_entries.size());
            for (size_t i = start_idx; i < end_idx; ++i) {
                const leaderboard_entry entry = leaderboard_entries[i];

                const int level = xp::calculator::level_from_xp(entry.xp);
                const Rank& rank = rankutil::rank_from_level(level);

                dpp::guild_member member = entry.member;
                auto user = member.get_user();

                std::string nickname = member.get_nickname();

                leaderboard_embed.add_component_v2(
                    dpp::component()
                        .set_type(dpp::cot_text_display)
                        .set_content(std::format("**{} - {}**\n@{}\nLevel {} • {} ({} XP)", entry.pos, nickname.empty() ? user->global_name : nickname, user->username, level, rank.name, util::format_with_commas(entry.xp)))
                );

                if (i + 1 != end_idx) {
                    leaderboard_embed.add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_separator)
                            .set_spacing(dpp::sep_small)
                            .set_divider(true)
                    );
                }
            }

            co_await event.co_reply(dpp::message().add_component_v2(leaderboard_embed));
        }
    }
}
