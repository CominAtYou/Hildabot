#include <dpp/dpp.h>
#include <unordered_map>
#include <functional>
#include "store/purchase_confirmation.h"
#include "slashcommands/store.h"
#include "store/execute_purchase.h"

static const std::unordered_map<std::string, std::function<dpp::task<void>(const dpp::button_click_t&)>> button_handlers = {
    {"store_confirm_purchase", store::purchase_confirmation::execute},
    {"store_return_to_menu", slash_commands::store::button_execute},
    {"store_execute_purchase", store::execute_purchase}
};


namespace events {
    dpp::task<void> handle_button_click(const dpp::button_click_t& event) {
        const std::string command = event.custom_id.substr(0, event.custom_id.find(':'));

        if (button_handlers.find(command) != button_handlers.end()) {
            co_await button_handlers.at(command)(event);
        }
    }
}
