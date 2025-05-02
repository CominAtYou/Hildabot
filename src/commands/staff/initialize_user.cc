#include <dpp/dpp.h>
#include <regex>
#include <algorithm>
#include <format>
#include <iterator>
#include "util/command_permissions.h"
#include "db/user_entry.h"
#include "constants.h"
#include <bsoncxx/builder/basic/document.hpp>
#include "xp/xp_system_calculator.h"
#include "db/mongo_database.h"
#include "rank/rank_util.h"
#include "logging/logging.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace commands {
    namespace initialize_user {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if (!(co_await util::can_run_sensitive_commands(event.owner, event.msg.member))) {
                co_return;
            }

            if (args.size() < 2) {
                co_await event.co_send("A user ID and a level must be provided.");
                co_return;
            }

            int level;
            try {
                level = std::stoi(args[1]);
            }
            catch (const std::invalid_argument&) {
                // can't co_await inside of a catch block
                event.send("Invalid level provided.", [](const dpp::confirmation_callback_t& callback) {});
                co_return;
            }

            if (level < 0) {
                co_await event.co_send("Level must be a positive number.");
                co_return;
            }

            std::regex id_regex("^[0-9]{17,19}$");
            if (!std::regex_match(args[0], id_regex)) {
                co_await event.co_send("Invalid user ID provided.");
                co_return;
            }

            const dpp::snowflake user_id = std::stoull(args[0]);

            if (user_id == event.msg.author.id) {
                co_await event.co_send("You can't initialize yourself. That's cheating!");
                co_return;
            }

            auto callback = co_await event.owner->co_guild_get_member(BASE_GUILD_ID, user_id);
            if (callback.is_error()) {
                co_await event.co_send("Unable to locate a user in this server with the provided ID.");
                co_return;
            }

            dpp::guild_member member = callback.get<dpp::guild_member>();

            auto user_callback = co_await event.owner->co_user_get_cached(user_id);
            if (user_callback.is_error()) {
                co_await event.co_send("Unable to locate a user in this server with the provided ID.");
                co_return;
            }

            dpp::user_identified user = user_callback.get<dpp::user_identified>();

            UserEntry user_entry(member);
            const int current_level = user_entry.get_level();

            if (user_entry.get_xp() >= xp::calculator::minimum_xp_for_level(level)) {
                co_await event.co_send(std::format("The user specified already exceeds level {}. (They're currently level {})", level, current_level));
                co_return;
            }

            if (current_level >= 10 && std::find(args.begin(), args.end(), "--force") == args.end()) {
                dpp::embed warning_embed = dpp::embed{}
                    .set_color(WARNING_YELLOW)
                    .set_title("Hold up! You might have typed something wrong.")
                    .set_description(std::format("You're attempting to initialize <@{}>, but they're already level {}. **You may have specified the wrong user.**\n\nIf you're sure that this is the user you wanted, re-run your command with `--force` at the end.", user_id.str(), user_entry.get_level()));

                co_await event.co_send(dpp::message{}.add_embed(warning_embed));
                co_return;
            }

            const int xp = xp::calculator::minimum_xp_for_level(level);
            auto& db = MongoDatabase::get_database();

            db["users"].update_one(
                make_document(kvp("_id", args[0])),
                make_document(kvp("$set", make_document(
                    kvp("xp", xp)
                )))
            );

            const int rank_level = rankutil::rank_from_level(level).level;
            const int index = std::distance(rankutil::rank_levels, std::find(rankutil::rank_levels, rankutil::rank_levels + sizeof(rankutil::rank_levels) / sizeof(rankutil::rank_levels[0]), rank_level));

            for (int i = index; i >= 0; i--) {
                const int level = rankutil::rank_levels[i];
                const int64_t role_id = rankutil::rank_from_level(level).role_id;
                dpp::snowflake role = dpp::snowflake(role_id);

                if (std::find(member.get_roles().begin(), member.get_roles().end(), role) == member.get_roles().end()) {
                    member.add_role(role);
                }
            }

            auto update_callback = co_await event.owner->co_guild_edit_member(member);
            if (update_callback.is_error()) {
                co_await event.co_send("The user has been initialized, but there was an error when trying to update their roles. Ask them to run `h!levelcheck` to restore their roles.");
            }
            else {
                co_await event.owner->co_message_add_reaction(event.msg.id, event.msg.channel_id, "👍");
            }

            co_await logging::event(event.owner, "UserInit", "{} ({}) was initialized to level {} by {} ({})", user.username, user.id.str(), level, event.msg.author.username, event.msg.author.id.str());
        }
    }
}
