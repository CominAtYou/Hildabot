#include <dpp/dpp.h>
#include "config.h"
#include "constants.h"
#include "message_create.h"
#include "xp/xp_system.h"
#include "commands/text_command_processor.h"

bool test_message_validity(const dpp::message& message) {
    if (message.author.is_bot()) return false;
    if (message.webhook_id != 0) return false;
    if (message.type != dpp::mt_default && message.type != dpp::mt_reply) return false;
    if (message.content.empty() && message.attachments.empty()) return false;
    if (message.is_dm() && (message.content == "h!levelalert" || message.content == "h!streakwarning")) return true;
    if (!message.guild_id.empty() && message.guild_id != BASE_GUILD_ID) return false;

    return true;
}

namespace events {
    dpp::task<void> handle_message_create(const dpp::message_create_t& event) {
        if (!test_message_validity(event.msg)) co_return;

        if (!event.msg.content.starts_with(PREFIX) && !event.msg.guild_id.empty() && event.msg.guild_id == BASE_GUILD_ID) {
            // Standard XP-eligible message
            co_await xp::give_xp_for_message(event);
        }
        else {
            co_await commands::run_command(event);
        }
    }
}
