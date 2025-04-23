#include <dpp/dpp.h>
#include <string>
#include <sstream>
#include "config.h"
#include <unordered_map>

std::unordered_map<std::string, std::function<dpp::task<void>(const dpp::message_create_t&)>> command_map = {

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
            co_await command_map[command](event);
        }
    }

}
