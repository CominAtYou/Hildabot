#include "apply_streak_savers.h"
#include <dpp/dpp.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <chrono>
#include "db/user_entry.h"
#include "db/mongo_database.h"
#include "util/helpers.h"
#include "constants.h"
#include "logging/logging.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

// "effective streak expiry"
namespace routine_tasks {
    void apply_streak_savers(dpp::cluster& bot) {
        logging::event(&bot, "StreakSavers", "Starting streak savers task.");

        const int64_t today_midnight = util::midnight_today_seconds();

        auto cursor = MongoDatabase::get_database()["users"].find(
            make_document(kvp("streak.expiry", today_midnight))
        );

        int hits = 0;
        for (const auto& doc : cursor) {
            const std::string_view user_id = doc["_id"].get_string();
            UserEntry user_entry(user_id);

            const int standard_count = doc["items"]["streak_savers"]["standard"].get_int32();
            const int super_count = doc["items"]["streak_savers"]["super"].get_int32();

            if (super_count == 0 && standard_count == 0) continue;

            hits++;

            const int64_t new_expiry = util::midnight_seconds_in_days(super_count > 0 ? 7 : 3);

            MongoDatabase::get_database()["users"].update_one(
                make_document(kvp("_id", user_id)),
                make_document(kvp("$set", make_document(
                    kvp("streak.expiry", new_expiry)
                )))
            );

            if (super_count > 0) {
                MongoDatabase::get_database()["users"].update_one(
                    make_document(kvp("_id", user_id)),
                    make_document(kvp("$inc", make_document(
                        kvp("items.streak_savers.super", -1)
                    )))
                );
            }
            else {
                MongoDatabase::get_database()["users"].update_one(
                    make_document(kvp("_id", user_id)),
                    make_document(kvp("$inc", make_document(
                        kvp("items.streak_savers.standard", -1)
                    )))
                );
            }

            if (user_entry.get_streak_warnings_preference()) {
                dpp::message msg = dpp::message()
                    .add_component_v2(
                        dpp::component()
                            .set_type(dpp::cot_container)
                            .set_accent(HILDA_BLUE)
                            .add_component_v2(
                                dpp::component()
                                    .set_type(dpp::cot_text_display)
                                    .set_content(std::format("## Your streak has been extended by {} days!\nYour streak now expires on <t:{}>.", super_count > 0 ? 7 : 3, new_expiry))
                            )
                            .add_component_v2(
                                dpp::component()
                                    .set_type(dpp::cot_separator)
                                    .set_spacing(dpp::sep_small)
                                    .set_divider(true)
                            )
                            .add_component_v2(
                                dpp::component()
                                    .set_type(dpp::cot_text_display)
                                    .set_content(std::format("-# To disable this warning in the future, run `h!streakwarning` in this DM or\n<#{}>", BOT_CHANNEL_ID))
                            )
                    );

                bot.direct_message_create(user_id, msg, [](const dpp::confirmation_callback_t& callback) {});
            }
        }

        logging::event(&bot, "Routine Tasks", "Applied streak savers to {} users.", hits);
    }
}
