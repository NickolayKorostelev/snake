#include "GameLogic.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include <vector>
#include <cstdlib>

Direction clampDirection(Direction current, Direction input) {
    if (input == Direction::None) return current;
    if (opposite(current) == input) return current;  // forbid reversing into neck
    return input;
}

Vec2i randomEmptyCell(const GameState& state) {
    std::vector<Vec2i> empty;
    empty.reserve(static_cast<size_t>(GRID_W) * static_cast<size_t>(GRID_H));
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            Vec2i c{ x, y };
            if (!state.snake.occupies(c) && !(c == state.food.position())) {
                empty.push_back(c);
            }
        }
    }
    if (empty.empty()) return { 0, 0 };
    return empty[static_cast<size_t>(std::rand()) % empty.size()];
}

GameState createInitialState() {
    GameState s;
    s.snake.init({ GRID_W / 2, GRID_H / 2 }, INITIAL_SNAKE_LEN, Direction::Right);
    s.food.respawn(s.snake, GRID_W, GRID_H, s.wrapWalls);
    return s;
}

void applyDifficulty(GameState& state, Difficulty d) {
    int idx = static_cast<int>(d);
    if (idx < 0 || idx > 2) idx = 1;
    state.tickRate = DIFFICULTY_TICK_RATE[idx];
    state.wrapWalls = DIFFICULTY_WRAP[idx];
}

TickResult tick(GameState& state, Direction input) {
    TickResult r;
    if (state.gameOver) return r;

    Direction newDir = clampDirection(state.snake.direction(), input);
    state.snake.setDirection(newDir);
    state.snake.move();

    if (state.wrapWalls) {
        state.snake.wrapHead(GRID_W, GRID_H);
    } else {
        Vec2i head = state.snake.head();
        if (head.x < 0 || head.x >= GRID_W || head.y < 0 || head.y >= GRID_H) {
            state.gameOver = true;
            r.gameOver = true;
            return r;
        }
    }

    // Self collision: head overlaps any non-head body segment.
    const auto& body = state.snake.body();
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i] == body.front()) {
            state.gameOver = true;
            r.gameOver = true;
            return r;
        }
    }

    if (state.snake.head() == state.food.position()) {
        state.snake.grow();
        state.food.respawn(state.snake, GRID_W, GRID_H, state.wrapWalls);
        state.score++;
        r.foodEaten = true;
        r.grew = true;

        if (state.score % SPEEDUP_EVERY == 0) {
            double next = state.tickRate * 0.9;
            state.tickRate = (next < MIN_TICK_RATE) ? MIN_TICK_RATE : next;
        }
    }

    return r;
}