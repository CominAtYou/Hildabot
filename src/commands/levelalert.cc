#include "levelalert.h"
#include <dpp/dpp.h>
#include <vector>
#include <string>
#include "constants.h"
#include "db/user_entry.h"
#include "db/mongo_database.h"
#include <bsoncxx/builder/basic/document.hpp>

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

static constexpr const char* LEVEL_ALERTS_DISABLED_MESSAGE = "Alright, level alerts have been disabled for you. If you want to turn them back on in the future, run this command again.";
static constexpr const char* LEVEL_ALERTS_ENABLED_MESSAGE = "Alright, level alerts have been enabled for you. If you want to turn them off in the future, run this command again.";

namespace commands {
    namespace levelalert {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            const dpp::snowflake channel_id = event.msg.channel_id;

            if (channel_id != BOT_CHANNEL_ID && channel_id != TESTING_CHANNEL_ID && !event.msg.is_dm()) {
                co_return;
            }

            UserEntry user(event.msg.author);
            // user_preference is an inversion of the current preference
            // TRUE = level_alerts_disabled = false
            // FALSE = level_alerts_disabled = true
            const bool user_preference = user.get_level_alerts_preference();

            MongoDatabase::get_database()["users"].update_one(
                make_document(kvp("_id", event.msg.author.id.str())),
                make_document(kvp("$set", make_document(kvp("level_alerts_disabled", user_preference))))
            );

            if (event.msg.is_dm()) {
                co_await event.co_send(user_preference ? LEVEL_ALERTS_DISABLED_MESSAGE : LEVEL_ALERTS_ENABLED_MESSAGE);
            }
            else {
                co_await event.co_reply(user_preference ? LEVEL_ALERTS_DISABLED_MESSAGE : LEVEL_ALERTS_ENABLED_MESSAGE);
            }
        }
    }
}
