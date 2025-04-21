#pragma once

#include <mongocxx/client.hpp>

class MongoDatabase {
public:
    static mongocxx::database& get_database();
};
