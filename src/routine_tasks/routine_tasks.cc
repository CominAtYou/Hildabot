#include "routine_tasks.h"
#include <dpp/dpp.h>
#include "constants.h"
#include "scheduler/instance.h"
#include "announce_birthdays.h"

namespace routine_tasks {
    void schedule(dpp::cluster& bot) {
        scheduler.cron("0 0 * * *", [&bot]() -> dpp::task<void> {
            co_await birthdays::announce(bot);
        });

        scheduler.cron("0 0 * * *", [&bot]() -> dpp::task<void> {
            co_return;
        });
    }
}
