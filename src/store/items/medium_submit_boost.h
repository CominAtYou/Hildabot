#pragma once
#include <dpp/dpp.h>
#include "store/store_item.h"

class medium_submit_boost : public store_item {
    public:
        medium_submit_boost() : store_item("3 Medium Submission Boosts", "Increase the XP you receive for your next 3 submissions by 25%.", "This item can only be purchased once every seven days.", "largesubmissionboost", 750, purchase_limit::WEEKLY) {}
        dpp::task<void> give_item(const dpp::button_click_t& event) const override;
};
