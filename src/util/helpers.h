#pragma once
#include <cstdint>

namespace util {
    int64_t seconds_since_epoch();
    int64_t midnight_today_seconds();
    int64_t midnight_tomorrow_seconds();
    int64_t midnight_seconds_in_a_week();
}
