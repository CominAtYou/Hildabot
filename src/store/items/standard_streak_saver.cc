#include "standard_streak_saver.h"
#include <dpp/dpp.h>
#include <chrono>
#include <format>
#include <bsoncxx/builder/basic/document.hpp>
#include "db/mongo_database.h"
#include "store/store.h"
#include "db/user_entry.h"
#include "util/helpers.h"
#include "db/migrate_user.h"
#include "constants.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

dpp::task<bool> standard_streak_saver::give_item(const dpp::button_click_t& event) const {
    if (!MongoDatabase::migrate_user(event.owner, event.command.usr)) {
        co_return false;
    }

    UserEntry user_entry(event.command.usr);

    std::chrono::zoned_time zt{"America/Chicago", std::chrono::system_clock::now()};

    std::chrono::local_days local_days = std::chrono::floor<std::chrono::days>(zt.get_local_time());
    std::chrono::local_days week_from_now = local_days + std::chrono::days(14);

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

    MongoDatabase::get_database()["users"].update_one(
        make_document(kvp("_id", event.command.usr.id.str())),
        make_document(kvp("$inc", make_document(
            kvp("items.streak_savers.standard", 1)
        )))
    );

    co_await this->send_success_message(event, "Your streak will be extended by 3 days should it expire.");
    co_return true;
}
