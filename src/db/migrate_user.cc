#include "migrate_user.h"
#include <dpp/dpp.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/exception/exception.hpp>
#include "db/user_entry.h"
#include "db/mongo_database.h"
#include "logging/logging.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::kvp;

namespace MongoDatabase {
    void migrate_v2(const dpp::snowflake& user_id) {
        auto& db = MongoDatabase::get_database();
        UserEntry user(user_id);

        if (user.get_version() == UserEntry::version::VERSION_2) {
            return; // No migration needed
        }

        db["users"].update_one(
            make_document(kvp("_id", user_id.str())),
            make_document(kvp("$set", make_document(
                kvp("items.streak_savers", make_document(
                    kvp("standard", 0),
                    kvp("super", 0)
                )),
                kvp("version", static_cast<int>(UserEntry::version::VERSION_2))
            )))
        );
    }

    void (*migration_functions[])(const dpp::snowflake&) = { [](const dpp::snowflake&){}, migrate_v2 };

    bool migrate_user(dpp::cluster* bot, const dpp::user& user) {
        UserEntry user_entry(user);

        if (user_entry.get_version() == LATEST_USER_ENTRY_VERSION) {
            return true; // No migration needed
        }

        const int version_number = static_cast<int>(user_entry.get_version());

        for (int i = version_number; i <= static_cast<int>(LATEST_USER_ENTRY_VERSION); ++i) {
            try {
                migration_functions[i](user.id);
            }
            catch (const mongocxx::exception& e) {
                logging::event(bot, "Migration", "Failed to migrate user {}: {}", user.id.str(), e.what());
                return user_entry.get_version() == LATEST_USER_ENTRY_VERSION;
            }
        }

        logging::event(bot, "Migration", "{} ({}) was migrated to version {}", user.username, user.id.str(), static_cast<int>(LATEST_USER_ENTRY_VERSION) + 1);
        return true;
    }
}
