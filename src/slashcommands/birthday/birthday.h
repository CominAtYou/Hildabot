#pragma once
#include <dpp/dpp.h>

namespace slash_commands {
    namespace birthday {
        dpp::task<void> execute(const dpp::slashcommand_t& event);
    }
}
