#pragma once
#include <dpp/dpp.h>

namespace slash_commands {
    namespace stats {
        dpp::task<void> execute(const dpp::slashcommand_t& event);
    }
}
