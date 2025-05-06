#pragma once
#include <dpp/dpp.h>

namespace slash_commands {
    namespace store {
        dpp::task<void> execute(const dpp::slashcommand_t& event);
        dpp::task<void> button_execute(const dpp::button_click_t& event);
    }
}
