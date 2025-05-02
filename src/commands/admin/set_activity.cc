#include "set_activity.h"
#include <dpp/dpp.h>
#include <vector>
#include <string>
#include "util/owner.h"

namespace commands {
    namespace set_activity {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if ((co_await util::get_owner_id(event.owner)) != event.msg.author.id) {
                co_return;
            }

            if (args.empty()) {
                co_await event.co_send("Please provide an argument.");
                co_return;
            }

            const std::string status = args[0];
            event.owner->set_presence(dpp::presence(dpp::ps_online, dpp::at_custom, status));

            co_await event.co_send(std::format("Set status to \"{}\"!", status));
        }
    }
}
