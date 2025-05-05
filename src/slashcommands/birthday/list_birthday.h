#pragma once
#include <dpp/dpp.h>

namespace slash_commands {
    namespace birthday {
        dpp::task<void> list(const dpp::slashcommand_t& event);
    }
}
