package com.cominatyou.commands;

import java.util.stream.Collectors;
import java.util.stream.IntStream;

import com.cominatyou.db.RedisUserEntry;
import com.cominatyou.util.Values;
import com.cominatyou.util.logging.Log;
import com.cominatyou.xp.RankUtil;

import org.javacord.api.entity.permission.Role;
import org.javacord.api.entity.user.User;
import org.javacord.api.event.message.MessageCreateEvent;

public class LevelCheck {
    public static void check(MessageCreateEvent message) { // Essentially the auto-restore, but command-based.
        if (message.getMessage().getChannel().getId() != Values.BOT_CHANNEL) return;

        final RedisUserEntry userEntry = new RedisUserEntry(message.getMessageAuthor());
        final User user = message.getMessageAuthor().asUser().get();

        if (userEntry.getXP() < 290) {
            message.getMessage().reply("You don't seem to be missing any roles, so you're all set!");
            return;
        }

        final int rankLevel = RankUtil.getRankFromLevel(userEntry.getLevel()).getLevel();

        final int[] rankLevels = RankUtil.getRanklevels();
        final int index = IntStream.of(rankLevels).boxed().collect(Collectors.toList()).indexOf(rankLevel);

        int restoredRoles = 0;
        for (int i = index; i > 0; i--) {
            final long levelId = RankUtil.getRankFromLevel(rankLevels[i]).getId();
            final Role role = message.getServer().get().getRoleById(levelId).get();
            if (!role.hasUser(user)) {
                role.addUser(user);
                restoredRoles++;
            }
        }

        if (restoredRoles > 0) {
            message.getMessage().reply("You're all set. The roles you were missing have been added to you.");
            Log.eventf("LEVELCHECK", "Restored %d role%s for %s (%d)\n", restoredRoles, (restoredRoles == 1 ? "" : "s"), message.getMessageAuthor().getDiscriminatedName(), message.getMessageAuthor().getId());
        }
        else {
            message.getMessage().reply("You don't seem to be missing any roles, so you're all set!");
        }
    }
}