#pragma once
#include <dpp/dpp.h>

namespace routine_tasks {
    dpp::task<void> announce_birthdays(dpp::cluster& bot);
}
