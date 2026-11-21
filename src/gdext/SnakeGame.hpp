#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include "core/GameState.hpp"
#include "core/GameLogic.hpp"

namespace snake {

// GDExtension bridge: owns the pure C++ GameState, drives fixed-timestep ticks
// from _process, reads input, and renders the board via _draw.
// UI (score, game over, pause, high score) is handled in GDScript via signals.
class SnakeGame : public godot::Node2D {
    GDCLASS(SnakeGame, Node2D)

    GameState _state;
    double _accumulator = 0.0;
    Difficulty _difficulty = Difficulty::Normal;
    Vec2i _lastEatenCell{};

protected:
    static void _bind_methods();

public:
    SnakeGame();
    ~SnakeGame() override = default;

    void _ready() override;
    void _process(double delta) override;
    void _draw() override;

    // Script-callable state queries.
    bool is_game_over() const { return _state.gameOver; }
    int get_score() const { return _state.score; }

    // Difficulty (0=Easy, 1=Normal, 2=Hard)
    void set_difficulty(int d);
    int get_difficulty() const { return static_cast<int>(_difficulty); }

    // Cell where the last food was eaten (for particle FX); (-1,-1) if none yet.
    godot::Vector2i get_last_eaten_cell() const;

private:
    Direction _read_input() const;
    void _on_tick();
    void _apply_difficulty();
};

}  // namespace snake