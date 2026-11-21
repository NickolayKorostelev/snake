#pragma once
#include "Types.hpp"
#include "Config.hpp"
#include "Snake.hpp"
#include "Food.hpp"

struct GameState {
    Snake snake;
    Food food;
    int score = 0;
    int level = 1;
    double tickRate = BASE_TICK_RATE;
    bool gameOver = false;
    bool wrapWalls = true;  // classic behavior: wrap around edges
};