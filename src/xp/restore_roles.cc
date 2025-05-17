#include "restore_roles.h"
#include <dpp/dpp.h>
#include <iterator>
#include "db/user_entry.h"
#include "rank/rank_util.h"

namespace restore_roles {
    dpp::task<void> restore(dpp::cluster* bot, dpp::guild_member member) {
        UserEntry user(member.user_id);
        if (user.get_xp() < 290) co_return;

        const int rank_level = rankutil::rank_from_level(user.get_level()).level;

        const int* begin = std::begin(rankutil::rank_levels);
        const int* end = std::end(rankutil::rank_levels);

        const int* elem_ptr = std::find(begin, end, rank_level);

        if (elem_ptr == end) co_return;

        for (const int* ptr = elem_ptr; elem_ptr >= rankutil::rank_levels; --ptr) {
            const int64_t role_id = rankutil::rank_from_level(*ptr).role_id;

            member.add_role(role_id);
        }

        co_await bot->co_guild_edit_member(member);
    }
}
