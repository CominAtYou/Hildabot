package com.cominatyou.commands;

import java.awt.Color;
import java.util.List;
import java.util.Optional;

import com.cominatyou.TextCommand;
import com.cominatyou.db.RedisUserEntry;
import com.cominatyou.util.Values;
import com.cominatyou.xp.RankUtil;
import com.cominatyou.xp.XPSystemCalculator;

import static com.cominatyou.util.ThousandsFormat.format;

import org.javacord.api.entity.message.embed.EmbedBuilder;
import org.javacord.api.entity.user.User;
import org.javacord.api.event.message.MessageCreateEvent;

public class Stats extends TextCommand {
    public void execute(MessageCreateEvent message, List<String> messageArgs) {
        if (!message.getMessageAuthor().isBotOwner() && message.getChannel().getId() != Values.BOT_CHANNEL) return;

        final String id;
        if (messageArgs.isEmpty()) {
            id = message.getMessageAuthor().getIdAsString();
        }
        else if (messageArgs.get(0).matches("[0-9]{17,19}")) {
            final String providedId = messageArgs.get(0);
            id = message.getServer().get().getMemberById(providedId).isEmpty() ? message.getMessageAuthor().getIdAsString() : providedId;
        }
        else {
            id = message.getMessageAuthor().getIdAsString();
        }

        final RedisUserEntry userEntry = new RedisUserEntry(id);
        final User user = message.getServer().get().getMemberById(id).get();

        final int currentLevel = userEntry.getLevel();
        final int currentStreak = userEntry.getInt("streak");

        final int xpForLevelUp =  XPSystemCalculator.determineMinimumUserFacingXPForLevel(currentLevel + 1);
        final int highScore = userEntry.getInt("highscore");

        final Optional<Color> roleColor = message.getServer().get().getRoleColor(user);

        final int numberOfFilledInCircles = Math.round((float) (userEntry.getXP() - (float) XPSystemCalculator.determineMinimumTotalXPForLevel(currentLevel)) / (float) xpForLevelUp * 20f);
        final StringBuilder progressCircles = new StringBuilder();

        for (int i = 0; i < numberOfFilledInCircles; i++) {
            progressCircles.append("●");
        }
        for (int i = 0; i < 20 - numberOfFilledInCircles; i++) {
            progressCircles.append("○");
        }

        final long currentRankRoleId = RankUtil.getRankFromLevel(currentLevel).getId();

        EmbedBuilder embed = new EmbedBuilder()
            .setTitle(user.getDisplayName(message.getServer().get()))
            .setThumbnail(user.getEffectiveAvatar(message.getServer().get()))
            .setColor(roleColor.orElse(Values.HILDA_BLUE))
            .setDescription((userEntry.hasKey("tagline") ? "**" + userEntry.getString("tagline") + "**\n" : "") + String.format("Level %d • %s", currentLevel, message.getServer().get().getRoleById(currentRankRoleId).get().getName()))
            .addField("Progress", progressCircles.toString())
            .addInlineField("XP", String.format("%s/%s", format(userEntry.getXP() - XPSystemCalculator.determineMinimumTotalXPForLevel(currentLevel)), format(xpForLevelUp)))
            .addInlineField("Streak", format(currentStreak))
            .addInlineField("High Score", format(highScore))
            .addField("Kudos", String.format("<:HildaStar:539313415425097728> **Given:** %s | <:HildaStar:539313415425097728> **Received:** %s", format(userEntry.getInt("kudos:given")), format(userEntry.getInt("kudos:received"))))
            .addField("Stats", String.format("**Submits:** %s | **Tokens:** %s", format(userEntry.getInt("timessubmitted")), format(userEntry.getInt("tokens"))))
            .addField("Submit Status", userEntry.getBoolean("submitted") ? (id.equals(message.getMessageAuthor().getIdAsString()) ? ":white_check_mark: You have submitted today!" : ":white_check_mark: Submitted today!") : (id.equals(message.getMessageAuthor().getIdAsString()) ? "You have not submitted today." : "Nothing yet today!"));

        final String streakExpiry = userEntry.getString("streakexpiry");
        if (streakExpiry != null) {
            embed.addField("Streak Expiry", "<t:" + streakExpiry + ">");
        }

        message.getChannel().sendMessage(embed);
    }
}
