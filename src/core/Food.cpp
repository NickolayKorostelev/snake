#include "Food.hpp"
#include "Snake.hpp"
#include <vector>
#include <cstdlib>

void Food::respawn(const Snake& snake, int gridW, int gridH, bool wrapWalls) {
    (void)wrapWalls;  // wall behavior does not affect where food can spawn
    if (gridW <= 0 || gridH <= 0) return;

    std::vector<Vec2i> empty;
    empty.reserve(static_cast<size_t>(gridW) * static_cast<size_t>(gridH));
    for (int y = 0; y < gridH; ++y) {
        for (int x = 0; x < gridW; ++x) {
            Vec2i c{ x, y };
            if (!snake.occupies(c)) empty.push_back(c);
        }
    }
    if (empty.empty()) return;  // board full: leave food where it is
    pos_ = empty[static_cast<size_t>(std::rand()) % empty.size()];
}