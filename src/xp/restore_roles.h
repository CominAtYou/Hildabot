#pragma once
#include <dpp/dpp.h>

namespace restore_roles {
    dpp::task<void> restore(dpp::cluster* bot, dpp::guild_member member);
}
