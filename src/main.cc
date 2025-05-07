#include <dpp/dpp.h>
#include <iostream>
#include "config.h"
#include "event_handlers/message_create.h"
#include "event_handlers/button_click.h"
#include "constants.h"
#include "activities/activity_swapper.h"
#include "slashcommands/slash_command_processor.h"

int main() {
    dpp::cluster bot(BOT_TOKEN);
    bot.intents = dpp::i_default_intents | dpp::i_message_content | dpp::i_guild_members;

    #ifdef DEBUG
    std::cout << "====== DEBUG BUILD ======\n";
    #endif

    bot.on_log(dpp::utility::cout_logger());

    // todo:
    // midnight tasks: birthdays, streak expiries, streak warnings
    // member joins
    // kudos tracking
    // leaderboard command
    // edit birthday command (API-side) to use integers instead of strings

    bot.on_ready([&bot](const dpp::ready_t& event) {
        std::cout << "Logged in as " << bot.me.format_username() << "\n";

        activity_swapper::start(event.owner);
    });

    bot.on_button_click([](const dpp::button_click_t& event) -> dpp::task<void> {
        co_await events::handle_button_click(event);
    });

    bot.on_message_create([](const dpp::message_create_t& event) -> dpp::task<void> {
        co_await events::handle_message_create(event);
    });

    bot.on_slashcommand([](const dpp::slashcommand_t& event) -> dpp::task<void> {
        co_await slash_commands::run_command(event);
    });

    bot.start(dpp::st_wait);
}

