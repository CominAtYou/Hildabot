#pragma once
#include <dpp/guild.h>
#include <cstdint>
#include <optional>
#include <string>

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

    /// @brief Get the number of seconds that represent midnight in a given number of days from now in the local timezone.
    /// @param days The number of days from now.
    /// @return The number of seconds since the Unix epoch that represents midnight in the given number of days from now in the local timezone.
    int64_t midnight_seconds_in_days(const int days);

    /// @brief Get the number of seconds that represent midnight in a week from now in the local timezone.
    /// @return The number of seconds since the Unix epoch that represents midnight in a week from now in the local timezone.
    int64_t midnight_seconds_in_a_week();

    /// @brief Format a number with commas as thousands separators.
    /// @tparam T The type of the number to format.
    /// @param value The number to format.
    /// @return A string representation of the number with commas as thousands separators.
    template<typename T>
    std::string format_with_commas(T value) {
        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));
        ss << std::fixed << value;
        return ss.str();
    }

    /// @brief Get a cached guild member from the bot's cache.
    /// @param bot The bot instance.
    /// @param user_id The user ID of the member to retrieve.
    /// @return An optional containing the guild member if found, or std::nullopt if not found.
    std::optional<dpp::guild_member> get_cached_guild_member(const dpp::cluster& bot, const dpp::snowflake& user_id);
}
