#pragma once
#include <dpp/dpp.h>

namespace store {
    namespace purchase_confirmation {
        dpp::task<void> execute(const dpp::button_click_t& event);
    }
}
