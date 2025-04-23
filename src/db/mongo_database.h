#pragma once

#include <mongocxx/client.hpp>
#include <bsoncxx/types.hpp>

#define DB_NULL bsoncxx::types::b_null{}

class MongoDatabase {
public:
    static mongocxx::database& get_database();
};
