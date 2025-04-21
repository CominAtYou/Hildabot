#include "mongo_database.h"
#include "config.h"

#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

mongocxx::database& MongoDatabase::get_database() {
    static mongocxx::instance driver_instance{};
    static mongocxx::client client(mongocxx::uri(MONGO_URI));
    static mongocxx::database db = client["hildabot"];

    return db;
}
