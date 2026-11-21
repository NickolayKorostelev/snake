#pragma once
#include "Types.hpp"

class Snake;

class Food {
    Vec2i pos_{};

public:
    Vec2i position() const { return pos_; }
    void setPosition(const Vec2i& p) { pos_ = p; }

    // Places food on a random empty (non-snake) cell within gridW x gridH.
    void respawn(const Snake& snake, int gridW, int gridH, bool wrapWalls);
};