#pragma once
#include <cstdint>

namespace util {
    /// @brief Get the number of seconds since the Unix epoch (1970-01-01).
    /// @return The number of seconds since the Unix epoch.
    int64_t seconds_since_epoch();
    /// @brief Get the number of seconds that represent midnight today in the local timezone.
    /// @return The number of seconds since the Unix epoch that represents midnight today in the local timezone.
    int64_t midnight_today_seconds();
    /// @brief Get the number of seconds that represent midnight tomorrow in the local timezone.
    /// @return The number of seconds since the Unix epoch that represents midnight tomorrow in the local timezone.
    int64_t midnight_tomorrow_seconds();
    /// @brief Get the number of seconds that represent midnight in a week from now in the local timezone.
    /// @return The number of seconds since the Unix epoch that represents midnight in a week from now in the local timezone.
    int64_t midnight_seconds_in_a_week();
    /// @brief Format a number with commas as thousands separators.
    /// @tparam T The type of the number to format.
    /// @param value The number to format.
    /// @return A string representation of the number with commas as thousands separators.
    template<typename T>
    std::string format_with_commas(T value);
}
