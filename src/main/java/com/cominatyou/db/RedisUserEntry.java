package com.cominatyou.db;

import com.cominatyou.xp.XPSystemCalculator;

public class RedisUserEntry {
    long id;
    String redisKey;

    public RedisUserEntry(long id) {
        this.id = id;
        this.redisKey = "users:" + id;
    }

    public long getID() {
        return id;
    }

    public String getRedisKey() {
        return redisKey;
    }

    public int getXP() {
        final String res = RedisInstance.getInstance().get(redisKey + ":xp");
        if (res == null) {
            RedisInstance.getInstance().set(redisKey + ":xp", "0");
            return 0;
        }
        return Integer.valueOf(res);
    }

    public int getLevel() {
        return XPSystemCalculator.determineLevelFromXP(getXP());
    }

    public boolean isEnrolled() {
        final String res = RedisInstance.getInstance().get(redisKey + ":enrolled");
        return !(res == null);
    }
}
