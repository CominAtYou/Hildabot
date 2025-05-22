#pragma once
#include <dpp/dpp.h>
#include "store/store_item.h"

class standard_streak_saver : public store_item {
    public:
        standard_streak_saver() : store_item("Streak Saver", "If your streak expires, 3 additional days will be added to your streak.", "This item can only be purchased once every fourteen days.", "standardstreaksaver", 1000, purchase_limit::BIWEEKLY) {}
        dpp::task<bool> give_item(const dpp::button_click_t& event) const override;
};
