#include "mongo_database.h"
#include "config.h"

#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace MongoDatabase {
    mongocxx::database& get_database() {
        static mongocxx::instance driver_instance{};
        static mongocxx::client client(mongocxx::uri(MONGO_URI));
        #ifndef DEBUG
        static mongocxx::database db = client["hildabot"];
        #else
        static mongocxx::database db = client["hildabot_test"];
        #endif

        return db;
    }
}
