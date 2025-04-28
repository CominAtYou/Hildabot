#pragma once
#include <cstdint>

struct Rank {
    const char* name;
    int level;
    uint64_t role_id;

    constexpr Rank(const char* n, int l, uint64_t r) : name(n), level(l), role_id(r) {}
};

namespace rankutil {
    static constexpr int rank_levels[] = {
        1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300
    };

    /// @brief Get the nearest minimum rank level for the specified level.
    /// @param level The level to get the rank level of.
    /// @return The minimum rank level for the level provided.
    int rank_level_from_level(int level);

    /// @brief Get the associated Rank object with a level.
    /// @param level The level to get the rank object of.
    /// @return The rank object for the level specified.
    const Rank& rank_from_level(int level);

    /// @brief Determine if the specified level is a level in which a rank is attained.
    /// @param level The level to test.
    /// @return `true` if the level is a level that gives a new rank, or `false` otherwise.
    bool is_level_rank_level(int level);
}
