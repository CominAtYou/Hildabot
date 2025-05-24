#pragma once
#include <array>
#include <memory>
#include <dpp/dpp.h>
#include "store_item.h"
#include "items/small_xp_increase.h"
#include "items/medium_submit_boost.h"
#include "items/small_submit_boost.h"
#include "items/medium_xp_increase.h"
#include "items/standard_streak_saver.h"
#include "items/super_streak_saver.h"

dpp::task<void> send_purchase_error_message(const dpp::button_click_t& event, const std::string& error_message);

namespace store_items {
    static const std::array<std::unique_ptr<store_item>, 6> items = {
        std::make_unique<small_xp_increase>(),
        std::make_unique<medium_xp_increase>(),
        std::make_unique<small_submit_boost>(),
        std::make_unique<medium_submit_boost>(),
        std::make_unique<standard_streak_saver>(),
        std::make_unique<super_streak_saver>()
    };

    const std::unique_ptr<store_item>& get_store_item(const std::string& id);
}
