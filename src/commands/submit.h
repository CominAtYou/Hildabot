#pragma once
#include <dpp/dpp.h>
#include <vector>

namespace commands {
    namespace submit {
        static constexpr uint64_t allowedChannels[] = {
            492578733442465804ULL,
            492579714674720778ULL,
            492580873628286976ULL,
            492580926111481859ULL,
            492885164993675264ULL,
            565327145786802176ULL,
            1139510301390282802ULL,
            1139510635223322674ULL,
            936669829979586580ULL /* testing channel ID */
        };

        dpp::task<void> execute(const dpp::message_create_t& event, const std::vector<std::string>& args);
    }
}
