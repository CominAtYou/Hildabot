#pragma once
#include <dpp/dpp.h>
#include "store/store_item.h"

class medium_xp_increase : public store_item {
    public:
        medium_xp_increase() : store_item("800 XP", "Instantly get an extra 800 XP!", "", "mediumxp", 1000, purchase_limit::UNLIMITED) {}
        dpp::task<bool> give_item(const dpp::button_click_t& event) const override;
};
