#pragma once
#include <dpp/dpp.h>
#include <vector>
#include <string>

namespace slash_commands {
    namespace inventory {
        dpp::task<void> execute(const dpp::slashcommand_t& event);
    }
}
