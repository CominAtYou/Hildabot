#pragma once
#include <dpp/dpp.h>

namespace kudos {
    dpp::task<void> tally(const dpp::message_reaction_add_t& event);
    dpp::task<void> remove(const dpp::message_reaction_remove_t& event);
}
