#include <dpp/dpp.h>
#include "set_birthay.h"

namespace slash_commands {
    namespace birthday {
        dpp::task<void> execute(const dpp::slashcommand_t& event) {
            dpp::command_interaction interaction = event.command.get_command_interaction();
            auto subcommand = interaction.options[0];

            if (subcommand.name == "set") {
                co_await slash_commands::birthday::set(event);
            }
        }
    }
}
