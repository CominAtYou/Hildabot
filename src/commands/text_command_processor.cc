#include <dpp/dpp.h>
#include <string>
#include <sstream>
#include <unordered_map>
#include "config.h"

#include "stats.h"
#include "levelcheck.h"
#include "submit.h"
#include "birthday.h"
#include "levelalert.h"

#include "staff/initialize_user.h"

#include "admin/set_activity.h"

typedef std::function<dpp::task<void>(const dpp::message_create_t&, const std::vector<std::string>&)> command_function;

static std::unordered_map<std::string, command_function> command_map = {
    {"submit", commands::submit::execute},
    {"stats", commands::stats::execute},
    {"levelcheck", commands::levelcheck::execute},
    {"birthday", commands::birthday::execute},
    {"inituser", commands::initialize_user::execute},
    {"setactivity", commands::set_activity::execute},
    {"levelalert", commands::levelalert::execute}
};

namespace commands {
    dpp::task<void> run_command(const dpp::message_create_t& event) {
        const std::string message_content_list = event.msg.content.substr(strlen(PREFIX));

        std::istringstream iss(message_content_list);
        std::vector<std::string> message_content_split;
        std::string elem;
        while (iss >> elem) {
            message_content_split.push_back(elem);
        }

        const std::string command = *message_content_split.begin();
        message_content_split.erase(message_content_split.begin());

        if (command_map.find(command) != command_map.end()) {
            co_await command_map[command](event, message_content_split);
        }
    }
}
