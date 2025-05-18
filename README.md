# Hildabot
Hildabot, built for Hildacord. Now in C++!

> [!IMPORTANT]
> This bot was built from the ground up to work with a specific Discord guild - as such, I provide no guarantees that it'll run properly in any guilds outside of the one it was designed for, or any help accomplishing such.

However, if you wish to run it in your own guild:
1. You'll need to install the projects dependencies - DPP and the MongoDB C++ driver. You should also have GNU make installed.
    - Instructions for installing DPP can be found [here](https://dpp.dev/installing.html), and instructions for the MongoDB driver can be found [here](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/get-started).
2. You'll need to swap out all of the IDs in the source for IDs in your server. A project-wide regex search in your IDE for Discord snowflakes (`[0-9]{17,}`) should identify anything that needs to be swapped out.
3. Set up a MongoDB database. This is required for things such as XP, submissions, birthdays, and the store.
    - The database should be named `hildabot` and have two collections — one named `users`, and another named `store`.
    - For more efficient operation, the `users` collection should have an index with key `xp` in descending order with background building enabled.
4. Fill out [`config.h`](https://github.com/CominAtYou/Hildabot/blob/main/src/include/config.h) and [`constants.h`](https://github.com/CominAtYou/Hildabot/blob/main/src/include/constants.h). You'll need to supply a prefix, your Discord bot token, a MongoDB URI, and various IDs for the channels and roles used by the bot.
5. Build with `make`.
