#include <dpp/dpp.h>
#include <algorithm>
#include "constants.h"
#include "util/owner.h"

namespace util {
    dpp::task<bool> can_run_sensitive_commands(dpp::cluster* bot, const dpp::guild_member& member) {
        auto roles = member.get_roles();

        co_return std::find(roles.begin(), roles.end(), 1115017832980303903ULL) != roles.end() || (co_await util::get_owner_id(bot)) == member.user_id;
    }
}
