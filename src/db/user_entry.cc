#include "user_entry.h"
#include "db/mongo_database.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <optional>
#include <utility>
#include <iterator>
#include <dpp/dpp.h>
#include "util/helpers.h"
#include "xp/xp_system_calculator.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::kvp;

UserEntry::UserEntry(const dpp::user& user) {
    this->user_id = user.id.str();
    create_entry_if_not_present(this->user_id);
}

UserEntry::UserEntry(const dpp::guild_member& member) {
    this->user_id = member.user_id.str();
    create_entry_if_not_present(this->user_id);
}

UserEntry::UserEntry(const dpp::snowflake& user_id) {
    this->user_id = user_id.str();
    create_entry_if_not_present(this->user_id);
}

void UserEntry::create_entry_if_not_present(const std::string user_id) {
    auto& db = MongoDatabase::get_database();
    auto result = db["users"].find_one(make_document(kvp("_id", user_id)));

    if (!result) {
        // User does not exist, create a new entry
        db["users"].insert_one(make_document(
            kvp("_id", user_id),
            kvp("kudos", make_document(
                kvp("received", 0),
                kvp("given", 0)
            )),
            kvp("tokens", 0),
            kvp("streak", make_document(
                kvp("expiry", DB_NULL),
                kvp("count", 0)
            )),
            kvp("recent_messages", make_document(
                kvp("count", 0),
                kvp("resets_at", DB_NULL)
            )),
            kvp("birthday", make_document(
                kvp("month", DB_NULL),
                kvp("day", DB_NULL)
            )),
            kvp("items", make_document(
                kvp("inventory", make_array()),
                kvp("submit_boosts", make_array()),
                kvp("streak_savers", make_document(
                    kvp("standard", 0),
                    kvp("mega", 0)
                ))
            )),
            kvp("times_submitted", 0),
            kvp("latest_submission_id", DB_NULL),
            kvp("streak_warnings_disabled", false),
            kvp("high_score", 0),
            kvp("xp", 0),
            kvp("level_alerts_disabled", false),
            kvp("version", static_cast<int>(UserEntry::version::VERSION_2))
        ));
    }
}

inline bsoncxx::document::value UserEntry::get_user_document() {
    auto& db = MongoDatabase::get_database();
    auto result = db["users"].find_one(make_document(kvp("_id", user_id)));
    return result.value();
}

int UserEntry::get_kudos_received() {
    return get_user_document()["kudos"]["received"].get_int32();
}

void UserEntry::increment_kudos_received() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$inc", make_document(kvp("kudos.received", 1))))
    );
}

void UserEntry::decrement_kudos_received() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$inc", make_document(kvp("kudos.received", -1))))
    );
}

int UserEntry::get_kudos_given() {
    return get_user_document()["kudos"]["given"].get_int32();
}

void UserEntry::increment_kudos_given() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$inc", make_document(kvp("kudos.given", 1))))
    );
}

void UserEntry::decrement_kudos_given() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$inc", make_document(kvp("kudos.given", -1))))
    );
}

int UserEntry::get_tokens() {
    return get_user_document()["tokens"].get_int32();
}

void UserEntry::increment_tokens(const int amount) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$inc", make_document(kvp("tokens", amount))))
    );
}

void UserEntry::decrement_tokens(const int amount) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$inc", make_document(kvp("tokens", -amount))))
    );
}

std::optional<int64_t> UserEntry::get_streak_expiry() {
    auto doc = get_user_document();
    auto field = doc["streak"]["expiry"];

    if (field.type() == bsoncxx::type::k_null) {
        return std::nullopt;
    }

    int64_t value = field.get_int64();

    if (value <= util::seconds_since_epoch()) {
        // If the expiry is in the past, reset the streak expiry
        MongoDatabase::get_database()["users"].update_one(
            make_document(kvp("_id", user_id)),
            make_document(kvp("$set", make_document(
                kvp("streak", make_document(
                    kvp("count", 0),
                    kvp("expiry", DB_NULL)
                )
            ))))
        );
        return std::nullopt;
    }

    return value;
}

int UserEntry::get_streak() {
    std::optional<int64_t> expiry = get_streak_expiry();

    if (expiry.has_value() && expiry <= util::seconds_since_epoch()) {
        // If the streak has expired, reset it
        MongoDatabase::get_database()["users"].update_one(
            make_document(kvp("_id", user_id)),
            make_document(kvp("$set", make_document(
                kvp("streak", make_document(
                    kvp("count", 0),
                    kvp("expiry", DB_NULL)
                )
            ))))
        );
        return 0;
    }

    return get_user_document()["streak"]["count"].get_int32();
}

std::pair<int, int64_t> UserEntry::process_submission(const dpp::snowflake& submission_id) {
    auto& db = MongoDatabase::get_database();
    int64_t week_after_now = util::midnight_seconds_in_a_week();
    const int64_t id = static_cast<int64_t>(static_cast<unsigned long>(submission_id));

    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$inc", make_document(kvp("streak.count", 1))))
    );

    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$set", make_document(kvp("streak.expiry", week_after_now))))
    );

    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$inc", make_document(kvp("times_submitted", 1))))
    );

    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$set", make_document(kvp("latest_submission_id", id))))
    );

    const int new_streak = get_streak();
    const int high_score = get_high_score();

    if (new_streak > high_score) {
        db["users"].update_one(
            make_document(kvp("_id", user_id)),
            make_document(kvp("$set", make_document(kvp("high_score", new_streak))))
        );
    }

    return {new_streak, week_after_now};
}

int UserEntry::get_recent_message_count() {
    auto doc = get_user_document();

    auto resets_at = doc["recent_messages"]["resets_at"];
    if (resets_at.type() == bsoncxx::type::k_null) {
        return 0;
    }

    if (resets_at.get_int64() <= util::seconds_since_epoch()) {
        // Reset the count if the reset time has passed
        reset_recent_message_count();
        return 0;
    }

    return doc["recent_messages"]["count"].get_int32();
}

void UserEntry::increment_recent_message_count() {
    auto& db = MongoDatabase::get_database();
    auto doc = get_user_document();

    int64_t seconds_since_epoch = util::seconds_since_epoch();

    auto resets_at = doc["recent_messages"]["resets_at"];

    // If resets_at is null, or is in the past, set it to 30 seconds from now
    if (resets_at.type() == bsoncxx::type::k_null || resets_at.get_int64() <= seconds_since_epoch) {
        db["users"].update_one(
            make_document(kvp("_id", user_id)),
            make_document(kvp("$set", make_document(
                kvp("recent_messages", make_document(
                    kvp("count", 1),
                    kvp("resets_at", seconds_since_epoch + 30)
                ))
            )))
        );
    }
    else {
        db["users"].update_one(
            make_document(kvp("_id", user_id)),
            make_document(kvp("$inc", make_document(kvp("recent_messages.count", 1))))
        );
    }
}

inline void UserEntry::reset_recent_message_count() {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$set", make_document(
            kvp("recent_messages", make_document(
                kvp("count", 0),
                kvp("resets_at", DB_NULL))
            )
        )))
    );
}

std::optional<std::pair<int, int>> UserEntry::get_birthday() {
    auto doc = get_user_document();
    auto birthday = doc["birthday"].get_document().view();
    auto month = birthday["month"];
    auto day = birthday["day"];

    if (month.type() == bsoncxx::type::k_null || day.type() == bsoncxx::type::k_null) {
        return std::nullopt;
    }

    return std::make_pair(month.get_int32(), day.get_int32());
}

std::optional<std::vector<double>> UserEntry::get_submit_boosts() {
    auto doc = get_user_document();
    auto items = doc["items"].get_document().view();

    if (items.find("submit_boosts") == items.end()) {
        return std::nullopt;
    }

    auto boosts = items["submit_boosts"].get_array().value;

    std::vector<double> boosts_vec;
    boosts_vec.reserve(3);

    for (size_t i = 0; i < boosts.length(); ++i) {
        boosts_vec.push_back(boosts[i].get_double());
    }

    return boosts_vec;
}

std::optional<double> UserEntry::shift_out_submit_boost() {
    auto& db = MongoDatabase::get_database();
    auto doc = get_user_document();
    auto items = doc["items"].get_document().view();

    if (items.find("submit_boosts") == items.end()) {
        return std::nullopt; // No boosts available
    }

    auto boosts = items["submit_boosts"].get_array().value;
    const int length = std::distance(boosts.begin(), boosts.end());

    if (length == 0) {
        return std::nullopt; // No boosts available
    }

    double first_boost = boosts[0].get_double();

    bsoncxx::builder::basic::array remaining_boosts;

    for (auto it = ++boosts.begin(); it != boosts.end(); ++it) {
        remaining_boosts.append(it->get_double());
    }

    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$set", make_document(kvp("items.submit_boosts", remaining_boosts))))
    );

    return first_boost;
}

int UserEntry::get_times_submitted() {
    return get_user_document()["times_submitted"].get_int32();
}

std::optional<dpp::snowflake> UserEntry::get_latest_submission_id() {
    auto field = get_user_document()["latest_submission_id"];

    if (field.type() == bsoncxx::type::k_null) {
        return std::nullopt;
    }

    return dpp::snowflake { field.get_int64().value };
}

bool UserEntry::get_streak_warnings_preference() {
    return !get_user_document()["streak_warnings_disabled"].get_bool();
}

int UserEntry::get_high_score() {
    return get_user_document()["high_score"].get_int32();
}

int UserEntry::get_xp() {
    return get_user_document()["xp"].get_int32();
}

void UserEntry::increment_xp(const int amount) {
    auto& db = MongoDatabase::get_database();
    db["users"].update_one(
        make_document(kvp("_id", user_id)),
        make_document(kvp("$inc", make_document(kvp("xp", amount))))
    );
}

bool UserEntry::has_submitted_today() {
    int midnight_today = util::midnight_today_seconds();
    auto doc = get_user_document();

    if (doc["latest_submission_id"].type() == bsoncxx::type::k_null) {
        return false; // No submission today
    }

    dpp::snowflake submission_id(doc["latest_submission_id"].get_int64().value);

    return submission_id.get_creation_time() >= midnight_today;
}

bool UserEntry::get_level_alerts_preference() {
    return !get_user_document()["level_alerts_disabled"].get_bool();
}

int UserEntry::get_level() {
    auto doc = get_user_document();
    int xp = doc["xp"].get_int32();
    return xp::calculator::level_from_xp(xp);
}

UserEntry::version UserEntry::get_version() {
    auto doc = get_user_document();
    return static_cast<UserEntry::version>((int) doc["version"].get_int32());
}
