#include "store.h"
#include <memory>
#include <string>

namespace store_items {
    const std::unique_ptr<store_item>& get_store_item(const std::string& id) {
        static const std::unique_ptr<store_item> NOT_FOUND = nullptr;

        for (const auto& item : items) {
            if (item->id == id) {
                return item;
            }
        }
        return NOT_FOUND;
    }
}
