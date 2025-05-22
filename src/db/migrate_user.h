#pragma once
#include <dpp/dpp.h>
#include "db/user_entry.h"

#define LATEST_USER_ENTRY_VERSION UserEntry::version::VERSION_2

namespace MongoDatabase {
    bool migrate_user(dpp::cluster* bot, const dpp::user& user);
}
