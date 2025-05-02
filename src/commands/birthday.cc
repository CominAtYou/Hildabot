#include "birthday.h"
#include <dpp/dpp.h>
#include <vector>
#include <string>
#include <format>

namespace commands {
    namespace birthday {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if (args.empty()) {
                co_await event.co_send("These commands have been deprecated. Please use the `/birthday` command instead.");
                co_return;
            }

            const std::string& arg = args[0];

            if (arg == "set" || arg == "edit" || arg == "list") {
                co_await event.co_send(std::format("This command has been deprecated. Please use </birthday {}:1011153003853643816> instead.", arg == "edit" ? "set" : arg));
            }
            else {
                co_await event.co_send("These commands have been deprecated. Please use the `/birthday` command instead.");
            }
        }
    }
}
