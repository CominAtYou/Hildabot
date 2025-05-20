#pragma once

#include <dpp/dpp.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <optional>
#include <vector>
#include <cstdint>
#include <string>
#include <utility>

class UserEntry {
    private:
        std::string user_id;
        void reset_recent_message_count();
        void create_entry_if_not_present(const std::string user_id);
        bsoncxx::document::value get_user_document();

        enum version {
            VERSION_1 // Initial version with basic fields
        };

        UserEntry::version get_version();
    public:
        UserEntry(const dpp::user& user);
        UserEntry(const dpp::guild_member& member);
        UserEntry(const dpp::snowflake& user_id);

        /// @brief Get the number of kudos received by the user.
        /// @return The number of kudos received.
        int get_kudos_received();
        /// @brief Increment the number of kudos received by the user.
        void increment_kudos_received();
        /// @brief Decrement the number of kudos received by the user.
        void decrement_kudos_received();

        /// @brief Get the number of kudos given by the user.
        /// @return The number of kudos given.
        int get_kudos_given();
        /// @brief Increment the number of kudos given by the user.
        /// @return The number of kudos given.
        void increment_kudos_given();
        /// @brief Decrement the number of kudos given by the user.
        /// @return The number of kudos given.
        void decrement_kudos_given();

        /// @brief Get the number of tokens the user has.
        /// @return The number of tokens the user has.
        int get_tokens();
        /// @brief Increment the number of tokens the user has by a specific amount.
        /// @param amount The amount to increment the tokens by.
        void increment_tokens(const int amount);
        /// @brief Decrement the number of tokens the user has by a specific amount.
        /// @param amount The amount to decrement the tokens by.
        void decrement_tokens(const int amount);

        /// @brief Get the user's current streak.
        /// @return The user's current streak. If they have no streak, this returns `0`.
        int get_streak();
        /// @brief Get the timestamp of when the user's streak expires.
        /// @return The timestamp of when the user's streak expires. If they have no streak, this returns `std::nullopt`.
        std::optional<int64_t> get_streak_expiry();
        /// @brief Process a submission and update the user's streak.
        /// @param submission_id The ID of the submission.
        /// @return A pair containing the new streak and the timestamp of when the streak expires.
        std::pair<int, int64_t> process_submission(const dpp::snowflake& submission_id);

        /// @brief Get the number of recent messages sent by the user in the last 30 seconds.
        /// @return The number of recent messages sent by the user in the last 30 seconds.
        int get_recent_message_count();
        /// @brief Increment the number of recent messages sent by the user.
        void increment_recent_message_count();

        /// @brief Get the user's birthday.
        /// @return A pair containing the month and day of the user's birthday. If the user has no birthday set, this returns `std::nullopt`.
        std::optional<std::pair<int, int>> get_birthday();

        /// @brief Get the user's available submit boosts.
        /// @return A vector of doubles representing the user's available submit boosts. If the user has no submit boosts, this returns `std::nullopt`.
        std::optional<std::vector<double>> get_submit_boosts();
        /// @brief Get the user's oldest submit boost.
        /// @return The user's oldest submit boost. If the user has no submit boosts, this returns `std::nullopt`.
        std::optional<double> shift_out_submit_boost();

        /// @brief Get the number of times the user has submitted.
        /// @return The number of times the user has submitted.
        int get_times_submitted();

        /// @brief Get the user's latest submission ID.
        /// @return The user's latest submission ID. If the user has never submitted, this returns `std::nullopt`.
        std::optional<dpp::snowflake> get_latest_submission_id();

        /// @brief Get the user's streak warnings setting.
        /// @return `true` if the user has streak warnings enabled, `false` otherwise.
        bool get_streak_warnings_preference();

        /// @brief Get the user's high score.
        /// @return The user's high score.
        int get_high_score();

        /// @brief Get the user's XP.
        /// @return The user's XP.
        int get_xp();
        /// @brief Increment the user's XP by a specific amount.
        /// @param amount The amount to increment the XP by.
        void increment_xp(const int amount);

        /// @brief Check if the user has submitted today.
        /// @return `true` if the user has submitted today, `false` otherwise.
        bool has_submitted_today();

        /// @brief Get the user's level alerts setting.
        /// @return `true` if the user has level alerts enabled, `false` otherwise.
        bool get_level_alerts_preference();

        /// @brief Get the user's level.
        /// @return The user's level.
        int get_level();

};
