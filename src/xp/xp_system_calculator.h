#pragma once

namespace xp {
    namespace calculator {
        /// @brief Determine the level for an XP value.
        /// @param xp The XP value, represented by a positive integer.
        /// @return The level for the XP value.
        int level_from_xp(int xp);

        /// @brief Determine the minimum total XP required to attain a level.
        /// @param level The level, represented by a positive integer.
        /// @return The minimum total XP required to attain the level.
        int minimum_xp_for_level(int level);

        /// @brief Determine the minimum XP required to level up from the previous level.
        /// @param level The level, represented by a positive integer.
        /// @return The minimum XP required to attain the level from the previous level.
        int minimum_user_facing_xp_for_level(int level);

    }
}
