#include <dpp/dpp.h>
#include <iostream>
#include <csignal>
#include "config.h"
#include "event_handlers/message_create.h"
#include "event_handlers/button_click.h"
#include "event_handlers/kudos.h"
#include "event_handlers/guild_member_update.h"
#include "event_handlers/guild_member_add.h"
#include "activities/activity_swapper.h"
#include "slashcommands/slash_command_processor.h"
#include "routine_tasks/routine_tasks.h"

static dpp::cluster bot(BOT_TOKEN);

int main() {
    bot.intents = dpp::i_default_intents | dpp::i_message_content | dpp::i_guild_members;

    #ifdef DEBUG
    std::cout << "====== DEBUG BUILD ======\n";
    #endif

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([](const dpp::ready_t& event) {
        std::cout << "Logged in as " << bot.me.format_username() << "\n";
        activity_swapper::start(event.owner);
        routine_tasks::schedule(bot);

        std::signal(SIGINT, [](int signal) {
            std::cout << "Interrupt received, shutting down...\n";
            bot.shutdown();
            exit(0);
        });
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

    bot.on_message_reaction_add([](const dpp::message_reaction_add_t& event) -> dpp::task<void> {
        co_await kudos::tally(event);
    });

    bot.on_message_reaction_remove([](const dpp::message_reaction_remove_t& event) -> dpp::task<void> {
        co_await kudos::remove(event);
    });

    bot.on_guild_member_add(guild_member_add::handle);

    bot.on_guild_member_update([](const dpp::guild_member_update_t& event) -> dpp::task<void> {
        co_await guild_member_update::handle(event);
    });

    bot.start(dpp::st_wait);
}

