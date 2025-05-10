#pragma once
#include <iostream>
#include <cstdarg>
#include <dpp/dpp.h>
#include <chrono>
#include <string>
#include <format>
#include "constants.h"

namespace logging {
    template<typename... Args>
    dpp::task<void> event(dpp::cluster* bot, const char* event_name, std::format_string<Args...> fmt, Args&&... args) {
        // get date and time in yyyy-MM-dd HH:mm:ss format

        auto now = std::chrono::system_clock::now();
        const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        const std::tm* now_tm = std::localtime(&now_time);
        char date_time[20];
        std::strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", now_tm);

        const std::string formatted_log = std::format(fmt, std::forward<Args>(args)...);
        std::string log_message = std::format("{} -- [{}] {}", date_time, event_name, formatted_log);
        std::cout << log_message << '\n';

        dpp::message message;
        message.set_content(log_message);
        message.set_channel_id(LOG_CHANNEL_ID);

        co_await bot->co_message_create(message);
    }

    template<typename... Args>
    dpp::task<void> error(dpp::cluster* bot, const char* event_name, std::format_string<Args...> fmt,  Args&&... args) {
        // get date and time in yyyy-MM-dd HH:mm:ss format
        auto now = std::chrono::system_clock::now();
        const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        const std::tm* now_tm = std::localtime(&now_time);
        char date_time[20];
        std::strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", now_tm);

        const std::string formatted_log = std::format(fmt, std::forward<Args>(args)...);
        std::string log_message = std::format("{} -- [{}] {}", date_time, event_name, formatted_log);
        std::cerr << log_message << '\n';

        dpp::embed embed;
        embed.set_title("Error");
        embed.set_color(ERROR_RED);
        embed.add_field("Event", event_name);
        embed.add_field("Message", formatted_log);

        dpp::message message;
        message.set_channel_id(LOG_CHANNEL_ID);
        message.add_embed(embed);

        co_await bot->co_message_create(message);
    }
}
