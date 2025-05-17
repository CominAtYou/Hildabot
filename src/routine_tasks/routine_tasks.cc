#include "routine_tasks.h"
#include <dpp/dpp.h>
#include "scheduler/instance.h"
#include "announce_birthdays.h"
#include "streak_warnings.h"

namespace routine_tasks {
    void schedule(dpp::cluster& bot) {
        scheduler.cron("0 0 * * *", [&bot]() -> dpp::task<void> {
            co_await announce_birthdays(bot);
        });

        scheduler.cron("0 0 * * *", [&bot]() -> dpp::task<void> {
            co_await streak_warnings(bot);
        });
    }
}
