#pragma once
#include <dpp/dpp.h>
#include "store/store_item.h"

class small_submit_boost : public store_item {
    public:
        small_submit_boost() : store_item("3 Small Submission Boosts", "Increase the XP you receive for your next 3 submissions by 10%.", "This item can only be purchased once every seven days.", "smallsubmissionboost", 500, purchase_limit::WEEKLY) {}
        dpp::task<void> give_item(const dpp::button_click_t& event) const override;
};
