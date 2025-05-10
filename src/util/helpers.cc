#include <chrono>
#include <string>
#include <iomanip>
#include <locale>

namespace util {
    int64_t seconds_since_epoch() {
        using namespace std::chrono;
        return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    }

    int64_t midnight_today_seconds() {
        std::chrono::zoned_time zt{"America/Chicago", std::chrono::system_clock::now()};
        auto local_time = zt.get_local_time();
        auto today_days = std::chrono::floor<std::chrono::days>(local_time);

        std::chrono::zoned_time midnight_zt{"America/Chicago", today_days};
        return std::chrono::duration_cast<std::chrono::seconds>(midnight_zt.get_sys_time().time_since_epoch()).count();
    }

    int64_t midnight_tomorrow_seconds() {
        std::chrono::zoned_time zt{"America/Chicago", std::chrono::system_clock::now()};
        auto local_time = zt.get_local_time();
        auto today_days = std::chrono::floor<std::chrono::days>(local_time);

        std::chrono::zoned_time tomorrow_midnight_zt{"America/Chicago", today_days + std::chrono::days(1)};
        return std::chrono::duration_cast<std::chrono::seconds>(tomorrow_midnight_zt.get_sys_time().time_since_epoch()).count();
    }

    int64_t midnight_seconds_in_days(const int days) {
        std::chrono::zoned_time zt{"America/Chicago", std::chrono::system_clock::now()};
        auto local_time = zt.get_local_time();
        auto today_days = std::chrono::floor<std::chrono::days>(local_time);

        std::chrono::zoned_time future_midnight_zt{"America/Chicago", today_days + std::chrono::days(days)};
        return std::chrono::duration_cast<std::chrono::seconds>(future_midnight_zt.get_sys_time().time_since_epoch()).count();
    }

    int64_t midnight_seconds_in_a_week() {
        std::chrono::zoned_time zt{"America/Chicago", std::chrono::system_clock::now()};
        auto local_time = zt.get_local_time();
        auto today_days = std::chrono::floor<std::chrono::days>(local_time);

        std::chrono::zoned_time next_week_midnight_zt{"America/Chicago", today_days + std::chrono::days(7)};
        return std::chrono::duration_cast<std::chrono::seconds>(next_week_midnight_zt.get_sys_time().time_since_epoch()).count();
    }
}
