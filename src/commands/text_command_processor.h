#pragma once
#include <dpp/dpp.h>

namespace commands {
    dpp::task<void> run_command(const dpp::message_create_t& event);
}
