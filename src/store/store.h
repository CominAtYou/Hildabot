#pragma once
#include <array>
#include <memory>
#include "store_item.h"
#include "items/small_xp_increase.h"
#include "items/medium_submit_boost.h"

namespace store_items {
    static const std::array<std::unique_ptr<store_item>, 2> items = {
        std::make_unique<small_xp_increase>(),
        std::make_unique<medium_submit_boost>(),
    };

    const std::unique_ptr<store_item>& get_store_item(const std::string& id);
}
