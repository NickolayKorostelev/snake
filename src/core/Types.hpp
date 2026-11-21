#pragma once
#include <cstdint>
#include <array>
#include <deque>

struct Vec2i {
    int x = 0;
    int y = 0;
};

inline bool operator==(const Vec2i& a, const Vec2i& b) { return a.x == b.x && a.y == b.y; }
inline bool operator!=(const Vec2i& a, const Vec2i& b) { return !(a == b); }

enum class Direction : uint8_t { Up, Down, Left, Right, None };

enum class CellType : uint8_t { Empty, Snake, Food, Wall };

// Indexed by Direction cast to int: Up=0, Down=1, Left=2, Right=3
constexpr Vec2i DIR_VECTORS[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

inline Direction opposite(Direction d) {
    switch (d) {
        case Direction::Up: return Direction::Down;
        case Direction::Down: return Direction::Up;
        case Direction::Left: return Direction::Right;
        case Direction::Right: return Direction::Left;
        default: return Direction::None;
    }
}

inline Vec2i dirVector(Direction d) {
    return DIR_VECTORS[static_cast<int>(d)];
}