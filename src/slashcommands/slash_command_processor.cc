#include <dpp/dpp.h>
#include <functional>
#include <unordered_map>
#include <string>
#include "birthday/birthday.h"
#include "stats.h"
#include "store.h"
#include "leaderboard.h"
#include "inventory.h"

typedef std::function<dpp::task<void>(const dpp::slashcommand_t&)> slash_command_function;

static std::unordered_map<std::string, slash_command_function> command_map = {
    {"birthday", slash_commands::birthday::execute},
    {"stats", slash_commands::stats::execute},
    {"store", slash_commands::store::execute},
    {"leaderboard", slash_commands::leaderboard::execute},
    {"inventory", slash_commands::inventory::execute}

};

namespace slash_commands {
    dpp::task<void> run_command(const dpp::slashcommand_t& event) {
        dpp::command_interaction interaction = event.command.get_command_interaction();
        const std::string command_name = interaction.name;

        if (command_map.find(command_name) != command_map.end()) {
            co_await command_map[command_name](event);
        }
    }
}
