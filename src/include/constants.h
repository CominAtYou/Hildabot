#pragma once

/// Color for regular embeds.
#define HILDA_BLUE 0x008080
/// Color for success embeds.
#define SUCCESS_GREEN 0x77B255
/// Color for error embeds.
#define ERROR_RED 0xED4245
/// Color for warning embeds.
#define WARNING_YELLOW 0xFEE75C
#ifndef DEBUG
/// A channel within the guild specified by BASE_GUILD_ID for running bot commands.
#define BOT_CHANNEL_ID 495034452422950915ULL
/// A channel ithin the guild specified by BASE_GUILD_ID that facilitates a testing environment for the bot.
#define TESTING_CHANNEL_ID 565327145786802176ULL
/// The guild where the bot will be active.
#define BASE_GUILD_ID 492572315138392064ULL
// The role for birthdays.
#define BIRTHDAY_ROLE_ID 609258045759029250ULL
// The channel for birthday announcements.
#define BIRTHDAY_CHANNEL_ID 609253148564914187ULL
#else
#define BOT_CHANNEL_ID 1366529081318838404ULL
#define TESTING_CHANNEL_ID 1366529081318838404ULL
#define BASE_GUILD_ID 936499529384091710ULL
#define BIRTHDAY_ROLE_ID 1370550921003536384ULL
#define BIRTHDAY_CHANNEL_ID 936848198423683072ULL
#endif
/// The channel for logging events.
#define LOG_CHANNEL_ID 1231339360348274709ULL
