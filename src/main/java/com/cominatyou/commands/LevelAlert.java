package com.cominatyou.commands;

import java.util.List;

import com.cominatyou.TextCommand;
import com.cominatyou.db.RedisUserEntry;
import com.cominatyou.util.MessageUtil;
import com.cominatyou.util.Values;

import org.javacord.api.entity.channel.ChannelType;
import org.javacord.api.event.message.MessageCreateEvent;

public class LevelAlert extends TextCommand {
    private final static String alertsDisabledMessage = "Alright, level alerts have been disabled for you. If you want to turn them back on in the future, run this command again.";
    private final static String alertsEnabledMessage = "Alright, level alerts have been enabled for you. If you want to turn them off in the future, run this command again.";

    public void execute(MessageCreateEvent message, List<String> args) {
        if (message.getChannel().getId() != Values.TESTING_CHANNEL && message.getChannel().getId() != Values.BOT_CHANNEL && message.getChannel().getType() != ChannelType.PRIVATE_CHANNEL) return;

        final RedisUserEntry user = new RedisUserEntry(message.getMessageAuthor());
        final boolean userPreference = user.getBoolean("levelalertsdisabled");

        user.set("levelalertsdisabled", userPreference ? "false" : "true");

        if (message.getChannel().getType() == ChannelType.PRIVATE_CHANNEL) {
            message.getChannel().sendMessage(userPreference ? alertsEnabledMessage : alertsDisabledMessage);
        }
        else {
            MessageUtil.sendTextReply(message.getMessage(), userPreference ? alertsEnabledMessage : alertsDisabledMessage);
        }
    }
}
