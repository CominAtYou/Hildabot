#include <dpp/dpp.h>
#include <optional>

namespace util {
    dpp::task<std::optional<dpp::snowflake>> get_owner_id(dpp::cluster* bot);
}
