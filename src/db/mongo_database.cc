#include "mongo_database.h"
#include "config.h"

#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

const mongocxx::instance driver_instance{};
const mongocxx::client client(mongocxx::uri(MONGO_URI));

#ifndef DEBUG
mongocxx::database db = client["hildabot"];
#else
mongocxx::database db = client["hildabot_test"];
#endif

// This is *not* thread-safe
namespace MongoDatabase {
    mongocxx::database& get_database() {
        return db;
    }
}
