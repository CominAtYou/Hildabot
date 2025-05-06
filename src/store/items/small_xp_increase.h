#pragma once
#include <dpp/dpp.h>
#include "store/store_item.h"

class small_xp_increase : public store_item {
    public:
        small_xp_increase() : store_item("150 XP", "Instantly get an extra 150 XP!", "", "smallxp", 450, purchase_limit::UNLIMITED) {}
        dpp::task<void> give_item(const dpp::button_click_t& event) const override;
};
