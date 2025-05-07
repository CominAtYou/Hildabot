#include "small_submit_boost.h"
#include <dpp/dpp.h>
#include <chrono>
#include <bsoncxx/builder/basic/document.hpp>
#include "db/user_entry.h"
#include "db/mongo_database.h"
#include "constants.h"
#include "xp/xp_system.h"
#include "util/helpers.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;

dpp::task<void> small_submit_boost::give_item(const dpp::button_click_t& event) const {
    UserEntry user_entry(event.command.usr);

    std::chrono::zoned_time zt{"America/Chicago", std::chrono::system_clock::now()};

    std::chrono::local_days local_days = std::chrono::floor<std::chrono::days>(zt.get_local_time());
    std::chrono::local_days week_from_now = local_days + std::chrono::days(7);

    std::chrono::zoned_time week_from_now_zt{ zt.get_time_zone(), week_from_now + (zt.get_local_time() - local_days) };
    int64_t week_from_now_seconds = std::chrono::duration_cast<std::chrono::seconds>(week_from_now_zt.get_sys_time().time_since_epoch()).count();

    auto store_doc = MongoDatabase::get_database()["store"].find_one(
        make_document(kvp("_id", event.command.usr.id.str()))
    );

    if (!store_doc) {
        MongoDatabase::get_database()["store"].insert_one(
            make_document(
                kvp("_id", event.command.usr.id.str()),
                kvp(this->id, make_document(kvp("purchasable_again", week_from_now_seconds)))
            )
        );
    }
    else {
        auto store_entry = store_doc->view();

        if (store_entry.find(this->id) == store_entry.end()) {
            MongoDatabase::get_database()["store"].update_one(
                make_document(kvp("_id", event.command.usr.id.str())),
                make_document(kvp("$set", make_document(kvp(this->id, make_document(kvp("purchasable_again", week_from_now_seconds))))))
            );
        }
        else {
            MongoDatabase::get_database()["store"].update_one(
                make_document(kvp("_id", event.command.usr.id.str())),
                make_document(kvp("$set", make_document(kvp(this->id + ".purchasable_again", week_from_now_seconds))))
            );
        }
    }

    user_entry.decrement_tokens(this->price);

    auto array = make_array<double>(1.1, 1.1, 1.1);

    MongoDatabase::get_database()["users"].update_one(
        make_document(kvp("_id", event.command.usr.id.str())),
        make_document(kvp("$push", make_document(
            kvp("items.submit_boosts", make_document(
                kvp("$each", array)
            )))))
    );

    co_await this->send_success_message(event, "Your next 3 submissions will be given a 10% increase in XP.");
}
