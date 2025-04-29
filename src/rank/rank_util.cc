#include "rank_util.h"
#include <cstdint>
#include <map>
#include <algorithm>
#include <stdexcept>

static std::map<int, Rank> rank_map = {
    #ifdef DEBUG
    {1, Rank("Time Worm", 1, 936499728970043453ULL)}, {5, Rank("Nitten", 5, 936499765569548308ULL)}, {10, Rank("Tide Mouse", 10, 936499819290177556ULL)},
    #else
    {1, Rank("Time Worm", 1, 505141598946852885ULL)}, {5, Rank("Nitten", 5, 492871026640814090ULL)}, {10, Rank("Tide Mouse", 10, 587846293091778560ULL)},
    #endif
    {15, Rank("Elf", 15, 644366736581459998ULL)},
    {20, Rank("Twig", 20, 492870780770713602ULL)}, {25, Rank("Woff", 25, 644366895231008788ULL)},
    {30, Rank("Vittra", 30, 644367091683688488ULL)}, {35, Rank("Nisse", 35, 587994460520972288ULL)},
    {40, Rank("Forest Giant", 40, 492871177648472077ULL)}, {45, Rank("Troll", 45, 644367516604432384ULL)},
    {50, Rank("Adventurer", 50, 498582087087947790ULL)}, {60, Rank("Sparrow Scout", 60, 498582101483061268ULL)},
    {70, Rank("Johanna", 70, 547261495231119370ULL)}, {80, Rank("Librarian", 80, 547263982151401482ULL)},
    {90, Rank("Marra", 90, 547261632703627274ULL)}, {100, Rank("Mad Scientist", 100, 587846803450363916ULL)},
    {120, Rank("Mountain Giant", 120, 644368570352009237ULL)}, {140, Rank("Weather Spirit", 140, 547261953421082635ULL)},
    {160, Rank("Thunder Bird", 160, 520057898731307009ULL)}, {180, Rank("Black Hound", 180, 587846612391297034ULL)},
    {200, Rank("Lindworm", 200, 644368923331919892ULL)}, {220, Rank("Erik", 220, 644369091901259776ULL)},
    {240, Rank("Alfur", 240, 644369272474173440ULL)}, {260, Rank("David", 260, 644369376274939916ULL)},
    {280, Rank("Frida", 280, 644369526129033217ULL)}, {300, Rank("Hilda", 300, 644369712108666903ULL)}
};

namespace rankutil {
    /// @brief Get the nearest minimum rank level for the specified level.
    /// @param level The level to get the rank level of.
    /// @return The minimum rank level for the level provided.
    int rank_level_from_level(int level) {
        // std::upper_bound returns the first element greater than the given level.
        auto it = std::upper_bound(std::begin(rank_levels), std::end(rank_levels), level);
        if (it == std::begin(rank_levels)) {
            // If the level is lower than the smallest rank level.
            throw std::out_of_range("Level is below the minimum rank level.");
        }
        // Step back one element to get the largest rank level <= level.
        return *(--it);
    }

    /// @brief Get the associated Rank object with a level.
    /// @param level The level to get the rank object of.
    /// @return The rank object for the level specified.
    const Rank& rank_from_level(int level) {
        return rank_map.at(rank_level_from_level(level));
    }

    /// @brief Determine if the specified level is a level in which a rank is attained.
    /// @param level The level to test.
    /// @return `true` if the level is a level that gives a new rank, or `false` otherwise.
    bool is_level_rank_level(int level) {
        return std::find(std::begin(rank_levels), std::end(rank_levels), level) != std::end(rank_levels);
    }
}
