#pragma once
#include <string>
#include <dpp/dpp.h>

enum purchase_limit {
    ONESHOT = -1,
    DAILY = 1,
    WEEKLY = 7,
    BIWEEKLY = 14,
    MONTHLY = 30,
    UNLIMITED = 0
};

class store_item {
    protected:
        dpp::task<void> send_success_message(const dpp::button_click_t& event, const std::string& message) const;
    public:
        std::string name;
        std::string description;
        std::string additional_info;
        std::string id;
        int price;
        enum purchase_limit purchase_limit;

        store_item(const std::string& name, const std::string& description, const std::string& additional_info, const std::string& id, int price, enum purchase_limit purchase_limit) {
            this->name = name;
            this->description = description;
            this->additional_info = additional_info;
            this->id = id;
            this->price = price;
            this->purchase_limit = purchase_limit;
        }

        virtual dpp::task<bool> give_item(const dpp::button_click_t& event) const = 0;
};
