package com.cominatyou.commands.admin;

import java.util.List;

import org.javacord.api.entity.channel.ChannelType;
import org.javacord.api.event.message.MessageCreateEvent;

public class SendTyping {
    public static void startTyping(MessageCreateEvent message, List<String> messageArgs) {
        if (!message.getMessageAuthor().isBotOwner()) return;

        message.deleteMessage();
        if (messageArgs.size() == 0) {
            message.getChannel().type();
            return;
        }

        final String channelId = messageArgs.get(0);

        message.getApi().getChannelById(channelId).ifPresent(channel -> {
            if (channel.getType() != ChannelType.SERVER_TEXT_CHANNEL) return;
            channel.asServerTextChannel().get().type();
        });
    }
}