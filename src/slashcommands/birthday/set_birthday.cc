#include <dpp/dpp.h>
#include <algorithm>
#include <bsoncxx/builder/basic/document.hpp>
#include <ctime>
#include <format>
#include "db/mongo_database.h"
#include "db/user_entry.h"
#include "birthday_constants.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace slash_commands {
    namespace birthday {
        dpp::task<void> set(const dpp::slashcommand_t& event) {
            dpp::command_interaction interaction = event.command.get_command_interaction();
            auto subcommand = interaction.options[0];

            const int month = subcommand.get_value<long>(0);
            const int day = subcommand.get_value<long>(1);

            if (std::binary_search(std::begin(thirty_day_months), std::end(thirty_day_months), month) && day > 30) {
                dpp::message reply = dpp::message{}
                    .set_content("That month only has 30 days! Double-check the date you provided.")
                    .set_flags(dpp::m_ephemeral);

                co_await event.co_reply(reply);
                co_return;
            }

            if (std::binary_search(std::begin(thirty_one_day_months), std::end(thirty_one_day_months), month) && day > 31) {
                dpp::message reply = dpp::message{}
                    .set_content("That month only has 31 days! Double-check the date you provided.")
                    .set_flags(dpp::m_ephemeral);

                co_await event.co_reply(reply);
                co_return;
            }

            if (month == 2 && day > 29) {
                dpp::message reply = dpp::message{}
                    .set_content("February only has 29 days (at most)! Double-check the date you provided.")
                    .set_flags(dpp::m_ephemeral);

                co_await event.co_reply(reply);
                co_return;
            }

            // needed to make sure the user entry exists
            UserEntry _(event.command.get_issuing_user());

            auto& db = MongoDatabase::get_database();
            db["users"].update_one(
                make_document(kvp("_id", event.command.get_issuing_user().id.str())),
                make_document(kvp("$set", make_document(kvp("birthday", make_document(
                    kvp("month", month),
                    kvp("day", day)
                )))))
            );

            time_t now = std::time(nullptr);
            tm* local_time = std::localtime(&now);

            int current_month = local_time->tm_mon + 1; // tm_mon is 0-based
            int current_day = local_time->tm_mday;

            if (month == current_month && day == current_day) {
                if (current_month == 2 && day == 29) {
                    dpp::message reply = dpp::message{ "Happy birthday! Your birthday has been set to February 29. Your birthday will be announced on March 1 on non-leap years." };
                    co_await event.co_reply(reply);
                }
                else {
                    dpp::message reply = dpp::message{ std::format("Happy birthday! Your birthday has been set to {} {}.", month_names[month - 1], day) };
                    co_await event.co_reply(reply);
                }
            }
            else if (month == 2 && day == 29) {
                dpp::message reply = dpp::message{ "Success! Your birthday has been set to February 29. Your birthday will be announced on March 1 on non-leap years." };
                co_await event.co_reply(reply);
            }
            else {
                dpp::message reply = dpp::message{ std::format("Success! Your birthday has been set to {} {}.", month_names[month - 1], day) };
                co_await event.co_reply(reply);
            }

        }
    }
}
