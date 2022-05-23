package com.cominatyou.commands.admin;

import java.util.Arrays;
import java.util.List;

import com.cominatyou.commands.Command;
import com.cominatyou.db.RedisUserEntry;
import com.cominatyou.xp.RankUtil;
import com.cominatyou.xp.XPSystemCalculator;

import org.javacord.api.entity.permission.Role;
import org.javacord.api.event.message.MessageCreateEvent;

public class InitializeUser extends Command {
    public void execute(MessageCreateEvent message, List<String> messageArgs) {
        if (!message.getMessageAuthor().isBotOwner()) return;
        if (messageArgs.size() < 2) {
            message.getChannel().sendMessage("You need to provide a user ID and/or a level for that!");
            return;
        }
        final Integer level;

        try {
            level = Integer.parseInt(messageArgs.get(1));
        } catch (NumberFormatException e) {
            message.getChannel().sendMessage("Invalid level provided.");
            return;
        }

        message.getServer().get().getMemberById(messageArgs.get(0)).ifPresentOrElse(user -> {
            final RedisUserEntry dbUser = new RedisUserEntry(user);

            final Integer xp = XPSystemCalculator.determineMinimumTotalXPForLevel(level);
            dbUser.set("xp", xp.toString());

            final int rankLevel = RankUtil.getRankFromLevel(dbUser.getLevel()).getLevel();
            final Integer[] rankLevels = RankUtil.getRanklevels();
            final int index = Arrays.asList(rankLevels).indexOf(rankLevel);

            // Give roles
            for (int i = index; i > 0; i--) {
                final long levelId = RankUtil.getRankFromLevel(rankLevels[i]).getId();
                final Role role = message.getServer().get().getRoleById(levelId).get();
                role.addUser(user);
            }

            message.getChannel().sendMessage("Completed successfully. The user should have all their XP and roles now.");
        }, () -> {
            message.getChannel().sendMessage("That ID doesn't seem to be the ID of someone in this server!");
        });
    }
}