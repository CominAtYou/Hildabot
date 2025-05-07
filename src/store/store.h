#pragma once
#include <array>
#include <memory>
#include "store_item.h"
#include "items/small_xp_increase.h"
#include "items/medium_submit_boost.h"
#include "items/small_submit_boost.h"
#include "items/medium_xp_increase.h"

namespace store_items {
    static const std::array<std::unique_ptr<store_item>, 4> items = {
        std::make_unique<small_xp_increase>(),
        std::make_unique<medium_xp_increase>(),
        std::make_unique<small_submit_boost>(),
        std::make_unique<medium_submit_boost>()
    };

    const std::unique_ptr<store_item>& get_store_item(const std::string& id);
}
