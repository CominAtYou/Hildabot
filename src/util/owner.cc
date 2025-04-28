#include "owner.h"
#include <dpp/dpp.h>
#include <optional>
#include "logging/logging.h"

namespace util {
    dpp::task<std::optional<dpp::snowflake>> get_owner_id(dpp::cluster& bot) {
        static dpp::snowflake owner_id;
        static bool initialized = false;

        if (initialized) {
            co_return owner_id;
        }

        auto callback = co_await bot.co_current_application_get();

        if (callback.is_error()) {
            co_return std::nullopt;
        }

        dpp::application app = callback.get<dpp::application>();
        owner_id = app.owner.id;
        initialized = true;

        co_return owner_id;
    }
}
