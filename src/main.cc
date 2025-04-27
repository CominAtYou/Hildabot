#include <dpp/dpp.h>
#include "config.h"
#include "event_handlers/message_create.h"
#include <iostream>
#include "constants.h"

int main() {
    dpp::cluster bot(BOT_TOKEN);
    bot.intents = dpp::i_default_intents | dpp::i_message_content | dpp::i_guild_members;

    #ifdef DEBUG
    std::cout << "====== DEBUG BUILD ======\n";
    #endif

    bot.on_log(dpp::utility::cout_logger());

    // midnight tasks: birthdays, streak expiries, streak warnings

    bot.on_ready([&bot](auto event) {
        std::cout << "Logged in as " << bot.me.format_username() << "\n";
    });

    bot.on_message_create([](const dpp::message_create_t& event) -> dpp::task<void> {
        co_await events::handle_message_create(event);
    });

    bot.start(dpp::st_wait);
}

