#include "leaderboard.h"
#include <dpp/dpp.h>
#include "constants.h"

namespace commands {
    namespace leaderboard {
        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args) {
            if (event.msg.channel_id != BOT_CHANNEL_ID) co_return;
            co_await event.co_send("Please use </leaderboard:1373828050533224448> to display the leaderboard. Setting `relative` to True will allow you to see where you lie in the leaderboard, whilst False will display the top 5 members.");
        }
    }
}
