package com.cominatyou.xp;

import java.util.Arrays;
import java.util.HashMap;

public class RankUtil {
    private final static int[] rankLevels = { 1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300 };

    private final static Rank[] ranks = { new Rank("Time Worm", 1, 505141598946852885L),
            new Rank("Nitten", 5, 492871026640814090L), new Rank("Tide Mouse", 10, 587846293091778560L),
            new Rank("Elf", 15, 644366736581459998L), new Rank("Twig", 20, 492870780770713602L),
            new Rank("Woff", 25, 644366895231008788L), new Rank("Vittra", 30, 644367091683688488L),
            new Rank("Nisse", 35, 587994460520972288L), new Rank("Forest Giant", 40, 492871177648472077L),
            new Rank("Troll", 45, 644367516604432384L), new Rank("Adventurer", 50, 498582087087947790L),
            new Rank("Sparrow Scout", 60, 498582101483061268L), new Rank("Johanna", 70, 547261495231119370L),
            new Rank("Librarian", 80, 547263982151401482L), new Rank("Marra", 90, 547261632703627274L),
            new Rank("Mad Scientist", 100, 587846803450363916L), new Rank("Mountain Giant", 120, 644368570352009237L),
            new Rank("Weather Spirit", 140, 547261953421082635L), new Rank("Thunder Bird", 160, 520057898731307009L),
            new Rank("Black Hound", 180, 587846612391297034L), new Rank("Lindworm", 200, 644368923331919892L),
            new Rank("Erik", 220, 644369091901259776L), new Rank("Alfur", 240, 644369272474173440L),
            new Rank("David", 260, 644369376274939916L), new Rank("Frida", 280, 644369526129033217L),
            new Rank("Hilda", 300, 644369712108666903L)
    };

    private static final HashMap<Integer, Rank> ranksMap = new HashMap<>(ranks.length);

    static {
        for (int i = 0; i < ranks.length; i++) {
            ranksMap.put(rankLevels[i], ranks[i]);
        }
    }

    /**
     * Get the nearest minimum rank level for the specified level.
     * @param level The level to get the rank level of.
     * @return The minimum rank level for the level provided.
     */
    private static int getRankLevelFromLevel(int level) {
        if (Arrays.binarySearch(rankLevels, level) > -1) return level;

        int rankLevel = -1;
        for (int i : rankLevels) {
            if (i < level) {
                rankLevel = i;
            }
        }

        return rankLevel;
    }

    /**
     * Get the associated rank object with a level.
     * @param level The level to get the rank object of.
     * @return The rank object for the level specified.
     */
    public static Rank getRankFromLevel(int level) {
        return ranksMap.get(getRankLevelFromLevel(level));
    }

    /**
     * Determine if the specified level is a level in which a rank is attained.
     * @param level The level to test.
     * @return {@code true} if the level is a level that gives a new rank, or {@code false} otherwise.
     */
    public static boolean isLevelRankLevel(int level) {
        return ranksMap.containsKey(level);
    }

    /**
     * Get the rank levels array.
     * @return The rank levels array.
     */
    public static int[] getRanklevels() {
        return rankLevels;
    }
}
