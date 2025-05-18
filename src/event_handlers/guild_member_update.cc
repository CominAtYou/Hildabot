#include "guild_member_update.h"
#include <dpp/dpp.h>
#include <random>
#include <format>
#include <algorithm>
#include "constants.h"
#include "xp/restore_roles.h"
#include "event_handlers/guild_member_add.h"

#include <iostream>

namespace guild_member_update {
    const char* welcome_messages[5] = { "Come on in and have a seat around the fire!", "We're glad to have you here!", "Glad you could make it!", "Make yourself at home!", "The bells toll to welcome your arrival." };

    dpp::task<void> handle(const dpp::guild_member_update_t& event) {
        auto it = std::find(pending_guild_members.begin(), pending_guild_members.end(), event.updated.user_id);
        if (it == pending_guild_members.end()) co_return;

        pending_guild_members.erase(it);
        std::cout << "Member removed: " << event.updated.user_id.str() << "\n";

        const dpp::embed embed = dpp::embed()
            .set_author({ event.updated.get_user()->global_name, "", event.updated.get_user()->get_avatar_url(0U, dpp::image_type::i_png, false) })
            .add_field("New to Discord?", "Don't worry! The folks at Discord HQ have got an [article](https://support.discord.com/hc/en-us/articles/360045138571-Beginner-s-Guide-to-Discord) to help you get up to speed!")
            .add_field("To get started, please read:", "- [Rules](https://discord.com/channels/492572315138392064/657825719522689045)\n- [Server Info](https://discord.com/channels/492572315138392064/657718082831384606)\n- [Plural Info](https://discord.com/channels/492572315138392064/787521898170810368)", true)
            .add_field("Channels to check out!", "- [Show Discussion](https://discord.com/channels/492572315138392064/492573040027369483)\n- [Movie Discussion](https://discord.com/channels/492572315138392064/498574984294301696)\n- [Fanart](https://discord.com/channels/492572315138392064/492580873628286976)", true)
            .set_image("https://media.giphy.com/media/ygBBMqVPvd4zvCRT1y/giphy.gif")
            .set_color(HILDA_BLUE)
            .set_footer({ "Have any questions? Ask around or check out #open-a-ticket for help!", event.updating_guild.get_icon_url(0U, dpp::image_type::i_png, false) });

        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 4);
        const int random_index = dis(gen);

        const std::string msg_content = std::format("**#{} - Welcome to Hildacord, {}** {}!\n\nHildacord is a great place for everyone from all around the world to come around a show that they all love: Hilda!", event.updating_guild.member_count, event.updated.get_user()->get_mention(), welcome_messages[random_index]);

        const dpp::message welcome_message = dpp::message(msg_content)
            .add_embed(embed)
            .set_channel_id(WELCOME_CHANNEL_ID);

        co_await event.owner->co_message_create(welcome_message);
        co_await restore_roles::restore(event.owner, event.updated);
    }
}
