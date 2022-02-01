package com.cominatyou.xp;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ThreadLocalRandom;

import com.cominatyou.db.RedisInstance;
import com.cominatyou.db.RedisUserEntry;
import com.cominatyou.util.Values;

import org.javacord.api.entity.message.embed.EmbedBuilder;
import org.javacord.api.entity.permission.Role;
import org.javacord.api.event.message.MessageCreateEvent;

public class XPSystem {
    private final static List<Long> ignoredChannels = Arrays.asList(495034452422950915L);

    public static void giveXPForMessage(MessageCreateEvent message) {
        if (ignoredChannels.contains(message.getChannel().getId())) return;

        final RedisUserEntry user = new RedisUserEntry(message.getMessageAuthor().getId());
        final String redisKey = user.getRedisKey();

        // XP will only be granted for 7 messages sent within 30 seconds. This might
        // need to be increased.
        if (RedisInstance.getInt(redisKey + ":recentmessagecount") == 7) return;

        final int currentXP = user.getXP();
        final int currentLevel = XPSystemCalculator.determineLevelFromXP(currentXP);

        // Award XP.
        int amount = ThreadLocalRandom.current().nextInt(2, 4 + 1); // random number between 2 and 4, inclusive
        user.addXP(amount);
        // System.out.printf("XP awarded for %s (%d): %d XP\n", message.getMessageAuthor().getDiscriminatedName(), message.getMessageAuthor().getId(), amount);

        // Check if rate limit key exists. If not, create it, set it to 1, and have it
        // expire in 30 seconds.
        if (!RedisInstance.keyExists(redisKey + ":recentmessagecount")) {
            RedisInstance.getInstance().set(redisKey + ":recentmessagecount", "1");
            RedisInstance.getInstance().expire(redisKey + ":recentmessagecount", 30);
        }
        else {
            // Incrememnt the rate limit key by 1 (this does not reset the expiration timer)
            RedisInstance.getInstance().incr(redisKey + ":recentmessagecount");
        }

        checkForLevelUp(currentLevel, message);
    }

    public static void checkForLevelUp(int beforeActionLevel, MessageCreateEvent message) {
        final RedisUserEntry user = new RedisUserEntry(message.getMessageAuthor().getId());

        final int currentXP = user.getXP();
        final int currentLevel = XPSystemCalculator.determineLevelFromXP(currentXP);

        if (currentLevel > beforeActionLevel) {
            final String embedTitle = RankUtil.isLevelRankLevel(currentLevel) ? String.format("Congrats on leveling up! You've reached level **%d** and are now the **%s** rank!", currentLevel, RankUtil.getRankFromLevel(currentLevel).getName()) : String.format("Congrats on leveling up! You are now level **%d**! :tada:", currentLevel);

            if (!RedisInstance.getBoolean("users:" + user.getID() + ":levelalertsdisabled")) {
                final EmbedBuilder embed = new EmbedBuilder()
                        .setColor(new java.awt.Color(Values.CODE_BLUE))
                        .setTitle(embedTitle)
                        .setDescription("To disable this message going forward, run `h!levelalert` in <#495034452422950915>.");
                message.getMessageAuthor().asUser().get().openPrivateChannel().join().sendMessage(embed);
            }

            if (RankUtil.isLevelRankLevel(currentLevel)) {
                final long roleId = RankUtil.getRankFromLevel(currentLevel).getId();
                final Role role = message.getServer().get().getRoleById(roleId).get();
                message.getMessageAuthor().asUser().get().addRole(role);
            }
        }
    }
}
