#pragma once
#include <dpp/dpp.h>

namespace slash_commands {
    dpp::task<void> run_command(const dpp::slashcommand_t& event);
}
