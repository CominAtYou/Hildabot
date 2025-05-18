#include "guild_member_add.h"
#include <dpp/dispatcher.h>

namespace guild_member_add {
    void handle(const dpp::guild_member_add_t& event) {
        if (event.added.has_bypasses_verification() || event.added.get_user()->is_bot()) {
            return;
        }

        pending_guild_members.push_back(event.added.user_id);
    }
}
