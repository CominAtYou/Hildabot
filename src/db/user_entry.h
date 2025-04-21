#include <dpp/user.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <optional>
#include <vector>
#include <cstdint>
#include <utility>

class UserEntry {
    private:
        int64_t user_id;
        void reset_recent_message_count();
    public:
        UserEntry(const dpp::user& user);

        bsoncxx::document::value get_user_document();

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
        void set_streak_expiry(const int64_t expiry);
        void reset_streak_expiry();

        int get_streak();
        std::pair<int, int64_t> increment_streak();
        void reset_streak();

        int get_recent_message_count();
        void increment_recent_message_count();

        void get_birthday(); // TODO: determine the type for this
        void set_birthday(const int month, const int day);

        // submit boosts?
        std::optional<std::vector<float>> get_submit_boosts();
        void add_submit_boosts(const std::vector<float> boosts);
        std::optional<float> shift_out_submit_boost();

        int get_times_submitted();
        void increment_times_submitted();

        std::optional<dpp::snowflake> get_latest_submission_id();
        void set_latest_submission_id(dpp::snowflake submission_id);

        bool get_streak_warnings_state();
        void set_streak_warnings_state(bool state);

        int get_high_score();

        int get_xp();
        void increment_xp(const int amount);

        bool has_submitted_today();

        bool get_level_alerts_state();
        void set_level_alerts_state(bool state);

};
