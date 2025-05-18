#pragma once
#include <dpp/dpp.h>
#include <vector>

inline std::vector<int64_t> pending_guild_members;

namespace guild_member_add {
    void handle(const dpp::guild_member_add_t& event);
}
