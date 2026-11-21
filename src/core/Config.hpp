#pragma once

// Grid dimensions (cells)
constexpr int GRID_W = 40;
constexpr int GRID_H = 30;
constexpr int CELL_SIZE = 15;

// Snake
constexpr int INITIAL_SNAKE_LEN = 4;

// Timing
constexpr double BASE_TICK_RATE = 0.1;  // seconds per tick (100ms)
constexpr int SPEEDUP_EVERY = 5;        // foods eaten per speed level
constexpr double MIN_TICK_RATE = 0.04;  // fastest allowed tick (40ms)

// Difficulty presets
enum class Difficulty { Easy = 0, Normal = 1, Hard = 2 };

constexpr double DIFFICULTY_TICK_RATE[3] = { 0.14, BASE_TICK_RATE, 0.07 };
constexpr bool DIFFICULTY_WRAP[3] = { true, true, false };