#include "xp_system_calculator.h"

namespace xp {
    namespace calculator {
        int level_from_xp(int xp) {
            if (xp < 50) return 1;

            int level = 1;
            int xp_for_level = 50;

            while (xp >= xp_for_level) {
                level++;
                xp_for_level += 50 + 15 * (level - 1);
            }

            return level;
        }

        int minimum_xp_for_level(int level) {
            if (level < 2) return 0;

            int xp = 50;
            for (int i = 2; i <= level; i++) {
                xp += 50 + 15 * (i - 1);
            }

            return xp;
        }

        int minimum_user_facing_xp_for_level(int level) {
            if (level < 2) return 0;
            return 50 + 15 * (level - 2);
        }

    }
}
