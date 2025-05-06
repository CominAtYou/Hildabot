#pragma once
#include <dpp/dpp.h>
#include <vector>

namespace commands {
    namespace submit {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args);
    }
}
