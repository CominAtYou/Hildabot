#include <dpp/dpp.h>
#include "db/mongo_database.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <vector>
#include <string>
#include <iterator>
#include <format>
#include <algorithm>
#include "constants.h"
#include "birthday_constants.h"
#include "util/helpers.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace slash_commands {
    namespace birthday {
        dpp::task<void> list(const dpp::slashcommand_t& event) {
            co_await event.co_thinking();

            dpp::command_interaction interaction = event.command.get_command_interaction();
            auto subcommand = interaction.options[0];
            int month = subcommand.get_value<long>(0);

            auto& db = MongoDatabase::get_database();

            auto filter = make_document(kvp("birthday.month", month));
            auto sort = make_document(kvp("birthday.day", 1));

            auto cursor = db["users"].find(filter.view());
            const std::string month_string = month_names[month - 1];
            const std::string shortened_month_string = month_string.substr(0, 3);

            std::vector<bsoncxx::document::view> results(cursor.begin(), cursor.end());
            std::sort(results.begin(), results.end(), [](const bsoncxx::document::view& a, const bsoncxx::document::view& b) {
                return a["birthday"]["day"].get_int32().value < b["birthday"]["day"].get_int32().value;
            });

            if (results.size() == 0) {
                dpp::embed embed = dpp::embed{}
                    .set_color(HILDA_BLUE)
                    .set_title(":birthday: Birthdays for " + month_string)
                    .set_description("No birtdays for this month!");

                co_await event.co_edit_response(embed);
                co_return;
            }

            dpp::embed embed = dpp::embed{}
                .set_color(HILDA_BLUE)
                .set_title(":birthday: Birthdays for " + std::string(month_names[month - 1]))
                .set_footer(std::format("Birthdays in {}: {}", month_string, results.size()), "");

            for (size_t i = 0; i < results.size(); i++) {
                auto user_id = results[i]["_id"].get_string().value;
                auto birthday = results[i]["birthday"].get_document().view();
                int day = birthday["day"].get_int32().value;

                std::optional<dpp::guild_member> member_opt = util::get_cached_guild_member(*event.owner, user_id);
                dpp::guild_member member;

                if (!member_opt) {
                    auto member_callback = co_await event.owner->co_guild_get_member(BASE_GUILD_ID, user_id);
                    if (member_callback.is_error()) continue; // skip users not in the guild
                    member = member_callback.get<dpp::guild_member>();
                }
                else {
                    member = member_opt.value();
                }

                if (i == 18) {
                    if (results.size() > 19) {
                        const bool is_thirty_day_month = std::find(std::begin(thirty_day_months), std::end(thirty_day_months), month) != std::end(thirty_day_months);
                        const int day_count = month == 2 ? 29 : (is_thirty_day_month ? 30 : 31);
                        embed.add_field(std::format("{} more", results.size() - 18), std::format("up to {} {}", shortened_month_string, day_count));
                    }
                    break;
                }
                else {
                    embed.add_field(member.get_user()->global_name.empty() ? member.get_user()->username : member.get_user()->global_name, std::format("{} {}", shortened_month_string, day), true);
                }
            }

            co_await event.co_edit_response(embed);
        }
    }
}
