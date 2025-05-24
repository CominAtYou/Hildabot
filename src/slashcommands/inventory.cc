#include "inventory.h"
#include <dpp/dpp.h>
#include <format>
#include <bsoncxx/builder/basic/document.hpp>
#include "db/mongo_database.h"
#include "db/migrate_user.h"
#include "constants.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace slash_commands {
    namespace inventory {
        dpp::task<void> execute(const dpp::slashcommand_t& event) {
            if (!MongoDatabase::migrate_user(event.owner, event.command.usr)) {
                co_await event.co_reply("Something went wrong while trying to get your inventory. Try giving it another shot.");
                co_return;
            }

            auto doc = *MongoDatabase::get_database()["users"].find_one(
                make_document(kvp("_id", event.command.usr.id.str()))
            );

            auto items = doc["items"].get_document().view();

            const int standard_streak_saver_count = items["streak_savers"]["standard"].get_int32();
            const int super_streak_saver_count = items["streak_savers"]["super"].get_int32();
            int medium_submission_boost_count = 0;
            int small_submission_boost_count = 0;


            auto submit_boosts = items["submit_boosts"].get_array().value;

            for (const auto& boost : submit_boosts) {
                if (boost.get_double() == 1.25) {
                    medium_submission_boost_count++;
                }
                else if (boost.get_double() == 1.1) {
                    small_submission_boost_count++;
                }
            }

            dpp::embed embed = dpp::embed()
                .set_author(dpp::embed_author({ event.command.usr.global_name, "", event.command.usr.get_avatar_url(0U, dpp::image_type::i_png, false) }))
                .set_title("Inventory")
                .set_color(HILDA_BLUE)
                .add_field("Streak Savers", std::format("**Standard**: {}\n**Super**: {}", standard_streak_saver_count, super_streak_saver_count), true)
                .add_field("Submission Boosts", std::format("**Small**: {}\n**Medium**: {}", small_submission_boost_count, medium_submission_boost_count), true);

            co_await event.co_reply(embed);
        }
    }
}
