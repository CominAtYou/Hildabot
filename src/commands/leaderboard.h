#pragma once
#include <dpp/dpp.h>
#include <vector>
#include <string>

namespace commands {
    namespace leaderboard {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args);
    }
}
