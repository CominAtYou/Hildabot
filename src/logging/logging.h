#pragma once
#include <cstdarg>
#include <dpp/dpp.h>

namespace logging {
    template<typename... Args>
    dpp::task<void> event(dpp::cluster* bot, const char* event_name, const char* fmt, Args&&... args);
    
    template<typename... Args>
    dpp::task<void> error(dpp::cluster* bot, const char* event_name, const char* fmt, Args&&... args);
}
