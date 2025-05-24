#pragma once
#include <dpp/dpp.h>
#include "store/store_item.h"

class super_streak_saver : public store_item {
    public:
        super_streak_saver() : store_item("Super Streak Saver", "If your streak expires, 1 additional week will be added to your streak.", "This item can only be purchased once every fourteen days.", "superstreaksaver", 1300, purchase_limit::BIWEEKLY) {}
        dpp::task<bool> give_item(const dpp::button_click_t& event) const override;
};
