#include "user_entry.h"
#include "db/mongo_database.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <optional>
#include <chrono>
#include <deque>
#include <utility>
#include "util/helpers.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::kvp;

UserEntry::UserEntry(const dpp::user& user) {
    user_id = user.id;

    auto& db = MongoDatabase::get_database();
    auto result = db["users"].find_one(make_document(kvp("id", user_id)));

    if (!result.has_value()) {
        // User does not exist, create a new entry
        db["users"].insert_one(make_document(
            kvp("id", user_id),
            kvp("kudos", make_document(
                kvp("received", 0),
                kvp("given", 0)
            )),
            kvp("tokens", 0),
            kvp("streak", make_document(
                kvp("expiry", NULL),
                kvp("count", 0)
            )),
            kvp("recentmessages", make_document(
                kvp("count", 0),
                kvp("resets_at", NULL)
            )),
            kvp("birthday", make_document()),
            kvp("items", make_document()),
            kvp("times_submitted", 0),
            kvp("latest_submission_id", NULL),
            kvp("streak_warnings_disabled", false),
            kvp("high_score", 0),
            kvp("xp", 0),
            kvp("submitted", false),
            kvp("level_alerts_disabled", false)
        ));
    }
}

bsoncxx::document::value UserEntry::get_user_document() {
    auto& db = MongoDatabase::get_database();
    auto result = db["users"].find_one(make_document(kvp("id", user_id)));
    return result.value();
}

int UserEntry::get_kudos_received() {
    return get_user_document().view()["kudos"]["received"].get_int32();
}

void UserEntry::increment_kudos_received() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$inc", make_document(kvp("kudos.received", 1))))
    );
}

void UserEntry::decrement_kudos_received() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$inc", make_document(kvp("kudos.received", -1))))
    );
}

int UserEntry::get_kudos_given() {
    return get_user_document().view()["kudos"]["given"].get_int32();
}

void UserEntry::increment_kudos_given() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$inc", make_document(kvp("kudos.given", 1))))
    );
}

void UserEntry::decrement_kudos_given() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$inc", make_document(kvp("kudos.given", -1))))
    );
}

int UserEntry::get_tokens() {
    return get_user_document().view()["tokens"].get_int32();
}

void UserEntry::increment_tokens(const int amount) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$inc", make_document(kvp("tokens", amount))))
    );
}

void UserEntry::decrement_tokens(const int amount) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$inc", make_document(kvp("tokens", -amount))))
    );
}

std::optional<int64_t> UserEntry::get_streak_expiry() {
    auto doc = get_user_document();
    auto field = doc.view()["streak"]["expiry"];

    if (field.type() == bsoncxx::type::k_null) {
        return std::nullopt;
    }

    return field.get_int64();
}

void UserEntry::reset_streak_expiry() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$set", make_document(kvp("streak.expiry", NULL))))
    );
}

int UserEntry::get_streak() {
    return get_user_document().view()["streak"]["count"].get_int32();
}

std::pair<int, int64_t> UserEntry::increment_streak() {
    auto& db = MongoDatabase::get_database();
    int64_t week_after_now = util::midnight_today_seconds() + 604800;

    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$inc", make_document(kvp("streak.count", 1))))
    );

    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$set", make_document(kvp("streak.expiry", week_after_now))))
    );

    int new_streak = get_user_document().view()["streak"]["count"].get_int32();

    return {new_streak, week_after_now};
}

void UserEntry::reset_streak() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$set", make_document(kvp("streak.count", 0))))
    );
}

int UserEntry::get_recent_message_count() {
    auto doc = get_user_document();
    auto doc_view = doc.view();

    auto resets_at = doc_view["recentmessages"]["resets_at"];
    if (resets_at.type() == bsoncxx::type::k_null) {
        return 0;
    }

    if (resets_at.get_int64() <= util::seconds_since_epoch()) {
        // Reset the count if the reset time has passed
        reset_recent_message_count();
        return 0;
    }

    return doc_view["recentmessages"]["count"].get_int32();
}

void UserEntry::increment_recent_message_count() {
    auto& db = MongoDatabase::get_database();
    auto doc = get_user_document();
    auto doc_view = doc.view();

    int64_t seconds_since_epoch = util::seconds_since_epoch();

    auto resets_at = doc_view["recentmessages"]["resets_at"];

    // If resets_at is null, or is in the past, set it to 30 seconds from now
    if (resets_at.type() == bsoncxx::type::k_null || resets_at.get_int64() <= seconds_since_epoch) {
        db["users"].update_one(
            make_document(kvp("id", user_id)),
            make_document(kvp("$set", make_document(
                kvp("recentmessages.count", 1),
                kvp("recentmessages.resets_at", seconds_since_epoch + 30)
            )))
        );
    }
    else {
        db["users"].update_one(
            make_document(kvp("id", user_id)),
            make_document(kvp("$inc", make_document(kvp("recentmessages.count", 1))))
        );
    }
}

void UserEntry::reset_recent_message_count() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$set", make_document(
            kvp("recentmessages.count", 0),
            kvp("recentmessages.resets_at", NULL)
        )))
    );
}

void UserEntry::get_birthday() {
    // todo
}

void UserEntry::set_birthday(const int month, const int day) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$set", make_document(kvp("birthday", make_document(
            kvp("month", month),
            kvp("day", day)
        ))))));
}

std::optional<std::vector<float>> UserEntry::get_submit_boosts() {
    auto doc = get_user_document();
    auto items = doc.view()["items"].get_document().view();

    if (items.find("submit_boosts") == items.end()) {
        return std::nullopt;
    }

    auto boosts = items["submit_boosts"].get_array().value;

    std::vector<float> boosts_vec;
    boosts_vec.reserve(3);

    for (int i = 0; i < boosts.length(); ++i) {
        boosts_vec.push_back(boosts[i].get_double());
    }

    return boosts_vec;
}

void UserEntry::add_submit_boosts(const std::vector<float> boosts) {
    auto doc = get_user_document();
    auto items = doc.view()["items"].get_document().view();
    auto boosts_opt = items.find("submit_boosts");

    if (boosts_opt == items.end()) {
        // If no boosts exist, create a new array
        auto array = make_array(boosts.begin(), boosts.end());

        auto& db = MongoDatabase::get_database();
        db["users"].update_one(
            make_document(kvp("id", user_id)),
            make_document(kvp("$set", make_document(kvp("items.submit_boosts", array))))
        );

        return;
    }

    auto& db = MongoDatabase::get_database();
    auto array = make_array(boosts.begin(), boosts.end());
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$push", make_document(kvp("items.submit_boosts", array))))
    );

}

std::optional<float> UserEntry::shift_out_submit_boost() {
    auto& db = MongoDatabase::get_database();
    auto doc = get_user_document();
    auto items = doc.view()["items"].get_document().view();

    if (items.find("submit_boosts") == items.end()) {
        return std::nullopt; // No boosts available
    }

    auto boosts = items["submit_boosts"].get_array().value;

    if (boosts.length() == 0) {
        return std::nullopt; // No boosts available
    }

    std::deque<float> boosts_deque{};

    for (int i = 0; i < boosts.length() - 1; ++i) {
        boosts_deque.push_back(boosts[i].get_double());
    }

    float first_boost = boosts_deque.front();
    boosts_deque.pop_front();

    auto new_array = make_array(boosts_deque.begin(), boosts_deque.end());

    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$set", make_document(kvp("items.submit_boosts", new_array))))
    );

    return first_boost;
}

int UserEntry::get_times_submitted() {
    return get_user_document().view()["times_submitted"].get_int32();
}

void UserEntry::increment_times_submitted() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$inc", make_document(kvp("times_submitted", 1))))
    );
}

std::optional<dpp::snowflake> UserEntry::get_latest_submission_id() {
    auto field = get_user_document().view()["latest_submission_id"];

    if (field.type() == bsoncxx::type::k_null) {
        return std::nullopt;
    }

    return dpp::snowflake { field.get_int64().value };
}

void UserEntry::set_latest_submission_id(dpp::snowflake submission_id) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$set", make_document(kvp("latest_submission_id", (uint64_t) submission_id))))
    );
}

bool UserEntry::get_streak_warnings_state() {
    return get_user_document().view()["streak_warnings_disabled"].get_bool();
}

void UserEntry::set_streak_warnings_state(bool state) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$set", make_document(kvp("streak_warnings_disabled", state))))
    );
}

int UserEntry::get_high_score() {
    return get_user_document().view()["high_score"].get_int32();
}

int UserEntry::get_xp() {
    return get_user_document().view()["xp"].get_int32();
}

void UserEntry::increment_xp(const int amount) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$inc", make_document(kvp("xp", amount))))
    );
}

bool UserEntry::has_submitted_today() {
    return get_user_document().view()["submitted"].get_bool();
}

bool UserEntry::get_level_alerts_state() {
    return get_user_document().view()["level_alerts_disabled"].get_bool();
}

void UserEntry::set_level_alerts_state(bool state) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("id", user_id)),
        make_document(kvp("$set", make_document(kvp("level_alerts_disabled", state))))
    );
}

