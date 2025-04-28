#include "birthday.h"
#include <dpp/dpp.h>
#include <vector>
#include <string>
#include <format>

namespace commands {
    namespace birthday {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if (args.empty()) {
                dpp::message reply = dpp::message{}
                    .set_content("These commands have been deprecated. Please use the `/birthday` command instead.")
                    .set_channel_id(event.msg.channel_id);

                co_await event.owner->co_message_create(reply);
                co_return;
            }

            const std::string& arg = args[0];

            if (arg == "set" || arg == "edit" || arg == "list") {
                dpp::message reply = dpp::message{}
                    .set_content(std::format("This command has been deprecated. Please use </birthday {}:1011153003853643816> instead.", arg == "edit" ? "set" : arg))
                    .set_channel_id(event.msg.channel_id);

                co_await event.owner->co_message_create(reply);
            }
            else {
                dpp::message reply = dpp::message{}
                    .set_content("These commands have been deprecated. Please use the `/birthday` command instead.")
                    .set_channel_id(event.msg.channel_id);

                co_await event.owner->co_message_create(reply);
            }
        }
    }
}
