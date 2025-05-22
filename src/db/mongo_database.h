#pragma once

#include <mongocxx/client.hpp>
#include <bsoncxx/types.hpp>

#define DB_NULL bsoncxx::types::b_null{}

namespace MongoDatabase {
    mongocxx::database& get_database();
};
