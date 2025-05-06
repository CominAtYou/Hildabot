#pragma once
#include <dpp/dpp.h>

namespace events {
    dpp::task<void> handle_button_click(const dpp::button_click_t& event);
}
