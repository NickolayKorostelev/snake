#include <gtest/gtest.h>

#include "Types.hpp"
#include "Config.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "GameState.hpp"
#include "GameLogic.hpp"

namespace {

GameState makeState(int snakeLen = 4, Direction dir = Direction::Right) {
    GameState s;
    s.snake.init({ GRID_W / 2, GRID_H / 2 }, snakeLen, dir);
    s.food.setPosition({ 0, 0 });  // keep food out of the way; tests set it explicitly
    return s;
}

}  // namespace

TEST(GameLogic, InitialState_CorrectLengthAndPos) {
    GameState s = createInitialState();
    EXPECT_EQ(s.snake.size(), static_cast<size_t>(INITIAL_SNAKE_LEN));
    EXPECT_EQ(s.snake.head().x, GRID_W / 2);
    EXPECT_EQ(s.snake.head().y, GRID_H / 2);
    EXPECT_FALSE(s.gameOver);
    EXPECT_GT(s.score, -1);
}

TEST(Snake, Move_AdvancesHead_PopsTail) {
    Snake sn;
    sn.init({ 5, 5 }, 4, Direction::Right);
    Vec2i headBefore = sn.head();
    size_t lenBefore = sn.size();

    sn.move();

    EXPECT_EQ(sn.head().x, headBefore.x + 1);
    EXPECT_EQ(sn.head().y, headBefore.y);
    EXPECT_EQ(sn.size(), lenBefore);  // length unchanged when not growing
}

TEST(Snake, Grow_IncreasesLength) {
    Snake sn;
    sn.init({ 5, 5 }, 4, Direction::Right);
    sn.grow();
    sn.move();
    EXPECT_EQ(sn.size(), static_cast<size_t>(5));
}

TEST(Snake, Init_BodyTrailsBehindHead) {
    Snake sn;
    sn.init({ 10, 10 }, 4, Direction::Right);
    const auto& b = sn.body();
    ASSERT_EQ(b.size(), static_cast<size_t>(4));
    EXPECT_EQ(b[0].x, 10);  // head
    EXPECT_EQ(b[1].x, 9);   // trailing left
    EXPECT_EQ(b[2].x, 8);
    EXPECT_EQ(b[3].x, 7);
}

TEST(GameLogic, ClampDirection_Rejects180) {
    EXPECT_EQ(clampDirection(Direction::Up, Direction::Down), Direction::Up);
    EXPECT_EQ(clampDirection(Direction::Right, Direction::Left), Direction::Right);
    EXPECT_EQ(clampDirection(Direction::Left, Direction::Right), Direction::Left);
    EXPECT_EQ(clampDirection(Direction::Down, Direction::Up), Direction::Down);
}

TEST(GameLogic, ClampDirection_AllowsPerpendicularTurn) {
    EXPECT_EQ(clampDirection(Direction::Right, Direction::Up), Direction::Up);
    EXPECT_EQ(clampDirection(Direction::Right, Direction::Down), Direction::Down);
    EXPECT_EQ(clampDirection(Direction::Up, Direction::Left), Direction::Left);
}

TEST(GameLogic, ClampDirection_NoneKeepsCurrent) {
    EXPECT_EQ(clampDirection(Direction::Right, Direction::None), Direction::Right);
}

TEST(GameLogic, Tick_FoodEaten_GrowsAndRespawns) {
    GameState s = makeState(4, Direction::Right);
    s.food.setPosition({ s.snake.head().x + 1, s.snake.head().y });  // directly ahead

    TickResult r = tick(s, Direction::Right);

    EXPECT_TRUE(r.foodEaten);
    EXPECT_TRUE(r.grew);
    EXPECT_EQ(s.score, 1);
    // Food must not sit on the snake after respawn.
    EXPECT_FALSE(s.snake.occupies(s.food.position()));
}

TEST(GameLogic, Tick_SelfCollision_GameOver) {
    GameState s = makeState(5, Direction::Right);
    s.food.setPosition({ 0, 0 });

    // Drive the snake in a tight loop; with length 5 it runs into its own body.
    EXPECT_FALSE(tick(s, Direction::Right).gameOver);  // head (6,5)
    EXPECT_FALSE(tick(s, Direction::Down).gameOver);   // head (6,6)
    EXPECT_FALSE(tick(s, Direction::Left).gameOver);   // head (5,6)
    EXPECT_TRUE(tick(s, Direction::Up).gameOver);      // head (5,5) collides
}

TEST(GameLogic, Tick_WallCollision_GameOver) {
    GameState s = makeState(4, Direction::Right);
    s.wrapWalls = false;
    s.snake.init({ GRID_W - 1, 5 }, 4, Direction::Right);
    s.food.setPosition({ 0, 0 });

    TickResult r = tick(s, Direction::Right);

    EXPECT_TRUE(r.gameOver);
    EXPECT_TRUE(s.gameOver);
}

TEST(GameLogic, Tick_WallWrap_Works) {
    GameState s = makeState(4, Direction::Right);
    s.wrapWalls = true;
    s.snake.init({ GRID_W - 1, 5 }, 4, Direction::Right);
    s.food.setPosition({ 0, 0 });

    TickResult r = tick(s, Direction::Right);

    EXPECT_FALSE(r.gameOver);
    EXPECT_EQ(s.snake.head().x, 0);  // wrapped from right edge to left
}

TEST(GameLogic, FoodRespawn_NotOnSnake) {
    GameState s = createInitialState();
    for (const auto& cell : s.snake.body()) {
        EXPECT_NE(s.food.position(), cell);
    }
}

TEST(GameLogic, Speedup_DecreasesTickRate) {
    GameState s = makeState(4, Direction::Right);
    s.score = SPEEDUP_EVERY - 1;  // next food triggers a speed level
    s.tickRate = BASE_TICK_RATE;
    s.food.setPosition({ s.snake.head().x + 1, s.snake.head().y });

    TickResult r = tick(s, Direction::Right);

    EXPECT_TRUE(r.foodEaten);
    EXPECT_EQ(s.score, SPEEDUP_EVERY);
    EXPECT_LT(s.tickRate, BASE_TICK_RATE);
    EXPECT_GE(s.tickRate, MIN_TICK_RATE);
}

TEST(GameLogic, Speedup_ClampsToMinimum) {
    GameState s = makeState(4, Direction::Right);
    s.tickRate = MIN_TICK_RATE;
    s.score = SPEEDUP_EVERY - 1;
    s.food.setPosition({ s.snake.head().x + 1, s.snake.head().y });

    tick(s, Direction::Right);

    EXPECT_DOUBLE_EQ(s.tickRate, MIN_TICK_RATE);
}

TEST(GameLogic, Difficulty_PresetsApply) {
    GameState s;
    s.wrapWalls = false;
    s.tickRate = 0.01;

    applyDifficulty(s, Difficulty::Easy);
    EXPECT_TRUE(s.wrapWalls);
    EXPECT_DOUBLE_EQ(s.tickRate, DIFFICULTY_TICK_RATE[0]);

    applyDifficulty(s, Difficulty::Hard);
    EXPECT_FALSE(s.wrapWalls);
    EXPECT_DOUBLE_EQ(s.tickRate, DIFFICULTY_TICK_RATE[2]);

    applyDifficulty(s, Difficulty::Normal);
    EXPECT_TRUE(s.wrapWalls);
    EXPECT_DOUBLE_EQ(s.tickRate, DIFFICULTY_TICK_RATE[1]);
}

TEST(GameLogic, Difficulty_OutOfRangeFallsBackToNormal) {
    GameState s;
    s.wrapWalls = false;
    applyDifficulty(s, static_cast<Difficulty>(99));
    EXPECT_TRUE(s.wrapWalls);  // Normal wraps
    EXPECT_DOUBLE_EQ(s.tickRate, DIFFICULTY_TICK_RATE[1]);
}