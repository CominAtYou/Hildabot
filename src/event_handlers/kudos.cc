#include "kudos.h"
#include <dpp/dpp.h>
#include "db/user_entry.h"
namespace kudos {
    dpp::task<void> tally(const dpp::message_reaction_add_t& event) {
        if (event.reacting_emoji.id != 539313415425097728ULL) co_return;
        if (event.reacting_channel.is_dm()) co_return;
        if (event.reacting_user.is_bot()) co_return;

        auto callback = co_await event.owner->co_message_get(event.message_id, event.channel_id);
        if (callback.is_error()) co_return;

        auto message = callback.get<dpp::message>();
        if (message.author.id == event.reacting_user.id) co_return;

        if (message.author.is_bot() || (message.type != dpp::mt_reply && message.type != dpp::mt_default)) co_return;
        UserEntry reactor_user_entry(event.reacting_user);
        UserEntry message_user_entry(message.author);

        message_user_entry.increment_kudos_received();
        reactor_user_entry.increment_kudos_given();
    }

    dpp::task<void> remove(const dpp::message_reaction_remove_t& event) {
        if (event.reacting_emoji.id != 539313415425097728ULL) co_return;
        if (event.reacting_channel.is_dm()) co_return;

        auto user_callback = co_await event.owner->co_user_get_cached(event.reacting_user_id);
        if (user_callback.is_error()) co_return;

        auto user = user_callback.get<dpp::user_identified>();
        if (user.is_bot()) co_return;

        auto callback = co_await event.owner->co_message_get(event.message_id, event.channel_id);
        if (callback.is_error()) co_return;

        dpp::message message = callback.get<dpp::message>();
        if (message.author.id == event.reacting_user_id) co_return;

        if (message.author.is_bot() || (message.type != dpp::mt_reply && message.type != dpp::mt_default)) co_return;
        UserEntry reactor_user_entry(event.reacting_user_id);
        UserEntry message_user_entry(message.author);

        message_user_entry.decrement_kudos_received();
        reactor_user_entry.decrement_kudos_given();
    }
}
