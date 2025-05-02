#include "streakwarning.h"
#include <dpp/dpp.h>
#include <vector>
#include <string>
#include "constants.h"
#include "db/user_entry.h"
#include "db/mongo_database.h"
#include <bsoncxx/builder/basic/document.hpp>

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

static constexpr const char* WARNINGS_DISABLED_MESSAGE = "Sure thing. Streak warnings have been disabled for you. Should you want to turn them back on, you can re-run this command.";
static constexpr const char* WARNINGS_ENABLED_MESSAGE = "Alright! Level warnings have been enabled again. If you want to turn them off again, re-run this command.";

namespace commands {
    namespace streakwarning {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            const dpp::snowflake channel_id = event.msg.channel_id;

            if (channel_id != BOT_CHANNEL_ID && channel_id != TESTING_CHANNEL_ID && !event.msg.is_dm()) {
                co_return;
            }

            UserEntry user(event.msg.author);
            // user_preference is an inversion of the current preference
            // TRUE = streak_warnings_disabled = false
            // FALSE = streak_warnings_disabled = true
            const bool user_preference = user.get_streak_warnings_preference();

            MongoDatabase::get_database()["users"].update_one(
                make_document(kvp("_id", event.msg.author.id.str())),
                make_document(kvp("$set", make_document(kvp("streak_warnings_disabled", user_preference))))
            );

            if (event.msg.is_dm()) {
                co_await event.co_send(user_preference ? WARNINGS_DISABLED_MESSAGE : WARNINGS_ENABLED_MESSAGE);
            }
            else {
                co_await event.co_reply(user_preference ? WARNINGS_DISABLED_MESSAGE : WARNINGS_ENABLED_MESSAGE);
            }
        }
    }
}
