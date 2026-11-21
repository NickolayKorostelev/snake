#pragma once
#include "Types.hpp"
#include "Config.hpp"
#include "GameState.hpp"

struct TickResult {
    bool gameOver = false;
    bool foodEaten = false;
    bool grew = false;
};

// Returns `input` unless it is a 180-degree reversal of `current` or None.
Direction clampDirection(Direction current, Direction input);

// Returns a random empty cell that is neither snake nor current food.
Vec2i randomEmptyCell(const GameState& state);

// Advances the game by one tick using `input` as the desired direction.
TickResult tick(GameState& state, Direction input);

// Builds the initial game state (snake centered, food spawned).
GameState createInitialState();

// Applies a difficulty preset to an existing state (tick rate + wall behavior).
void applyDifficulty(GameState& state, Difficulty d);