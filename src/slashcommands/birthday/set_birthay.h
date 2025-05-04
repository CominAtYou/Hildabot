#pragma once
#include <dpp/dpp.h>

namespace slash_commands {
    namespace birthday {
        dpp::task<void> set(const dpp::slashcommand_t& event);
    }
}
