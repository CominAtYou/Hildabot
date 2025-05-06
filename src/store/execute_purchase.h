#pragma once
#include <dpp/dpp.h>

namespace store {
    dpp::task<void> execute_purchase(const dpp::button_click_t& event);
}
