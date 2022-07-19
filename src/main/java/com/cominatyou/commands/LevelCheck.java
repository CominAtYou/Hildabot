package com.cominatyou.commands;

import java.util.Arrays;
import java.util.List;

import com.cominatyou.db.RedisUserEntry;
import com.cominatyou.util.Values;
import com.cominatyou.util.logging.Log;
import com.cominatyou.xp.RankUtil;

import org.atteo.evo.inflector.English;
import org.javacord.api.entity.permission.Role;
import org.javacord.api.entity.user.User;
import org.javacord.api.event.message.MessageCreateEvent;

public class LevelCheck implements Command {
    public void execute(MessageCreateEvent message, List<String> args) { // Essentially the auto-restore, but command-based.
        if (message.getMessage().getChannel().getId() != Values.BOT_CHANNEL) return;

        final RedisUserEntry userEntry = new RedisUserEntry(message.getMessageAuthor());
        final User user = message.getMessageAuthor().asUser().get();

        if (userEntry.getXP() < 290) {
            message.getMessage().reply("You don't seem to be missing any roles, so you're all set!");
            return;
        }

        final int rankLevel = RankUtil.getRankFromLevel(userEntry.getLevel()).getLevel();

        final Integer[] rankLevels = RankUtil.getRanklevels();
        final int index = Arrays.asList(rankLevels).indexOf(rankLevel);

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
            Log.eventf("LEVELCHECK", "Restored %d %s for %s (%d)\n", restoredRoles, English.plural("role", restoredRoles), message.getMessageAuthor().getDiscriminatedName(), message.getMessageAuthor().getId());
        }
        else {
            message.getMessage().reply("You don't seem to be missing any roles, so you're all set!");
        }
    }

    public String getName() {
        return "LevelCheck";
    }
}
