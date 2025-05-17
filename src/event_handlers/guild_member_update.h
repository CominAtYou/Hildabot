#pragma once
#include <dpp/dpp.h>

namespace guild_member_update {
    dpp::task<void> handle(const dpp::guild_member_update_t& event);
}
