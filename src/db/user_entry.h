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
    public:
        UserEntry(const dpp::user& user);
        UserEntry(const dpp::guild_member& member);

        int get_kudos_received();
        void increment_kudos_received();
        void decrement_kudos_received();

        int get_kudos_given();
        void increment_kudos_given();
        void decrement_kudos_given();

        int get_tokens();
        void increment_tokens(const int amount);
        void decrement_tokens(const int amount);

        std::optional<int64_t> get_streak_expiry();
        void reset_streak_expiry();

        int get_streak();
        std::pair<int, int64_t> process_submission(const dpp::snowflake& submission_id);
        void reset_streak();

        int get_recent_message_count();
        void increment_recent_message_count();

        std::optional<std::pair<int, int>> get_birthday();

        // submit boosts?
        std::optional<std::vector<double>> get_submit_boosts();
        void add_submit_boosts(const std::vector<double> boosts);
        std::optional<double> shift_out_submit_boost();

        int get_times_submitted();

        std::optional<dpp::snowflake> get_latest_submission_id();

        bool get_streak_warnings_preference();
        void set_streak_warnings_preference(bool state);

        int get_high_score();

        int get_xp();
        void increment_xp(const int amount);

        bool has_submitted_today();

        bool get_level_alerts_preference();
        void set_level_alerts_preference(bool state);

        int get_level();

};
