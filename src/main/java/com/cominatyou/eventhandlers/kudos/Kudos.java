package com.cominatyou.eventhandlers.kudos;

import com.cominatyou.db.RedisUserEntry;

import org.javacord.api.entity.channel.ChannelType;
import org.javacord.api.event.message.reaction.ReactionAddEvent;
import org.javacord.api.event.message.reaction.ReactionRemoveEvent;

public class Kudos {
    public static void tally(ReactionAddEvent reaction) {
        if (reaction.getChannel().getType() == ChannelType.PRIVATE_CHANNEL) return;
        if (reaction.getEmoji() != reaction.getServer().get().getCustomEmojiById(539313415425097728L).get()) return;
        if (reaction.getUser().get() == reaction.getMessageAuthor().get().asUser().get()) return;

        final RedisUserEntry giver = new RedisUserEntry(reaction.getUser().get()); // Person who reacted to the message
        final RedisUserEntry reciever = new RedisUserEntry(reaction.getMessageAuthor().get()); // Author of message that was reacted to

        giver.incrementKey("kudos:given");
        reciever.incrementKey("kudos:received");
    }

    public static void remove(ReactionRemoveEvent event) {
        if (event.getChannel().getType() == ChannelType.PRIVATE_CHANNEL) return;
        if (event.getEmoji() != event.getServer().get().getCustomEmojiById(539313415425097728L).get()) return;

        final RedisUserEntry giver = new RedisUserEntry(event.getUser().get());
        final RedisUserEntry reciever = new RedisUserEntry(event.getMessageAuthor().get());

        giver.decrementKey("kudos:given");
        reciever.decrementKey("kudos:received");
    }
}