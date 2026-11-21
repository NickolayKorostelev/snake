#include "SnakeGame.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/core/method_bind.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

namespace snake {

void SnakeGame::_bind_methods() {
    ClassDB::bind_method(D_METHOD("is_game_over"), &SnakeGame::is_game_over);
    ClassDB::bind_method(D_METHOD("get_score"), &SnakeGame::get_score);
    ClassDB::bind_method(D_METHOD("set_difficulty", "d"), &SnakeGame::set_difficulty);
    ClassDB::bind_method(D_METHOD("get_difficulty"), &SnakeGame::get_difficulty);
    ClassDB::bind_method(D_METHOD("get_last_eaten_cell"), &SnakeGame::get_last_eaten_cell);

    ADD_SIGNAL(MethodInfo("food_eaten", PropertyInfo(Variant::INT, "new_score")));
    ADD_SIGNAL(MethodInfo("game_over", PropertyInfo(Variant::INT, "final_score")));
}

SnakeGame::SnakeGame() {
    _state = createInitialState();
    _apply_difficulty();
    _lastEatenCell = { -1, -1 };
}

void SnakeGame::_apply_difficulty() {
    applyDifficulty(_state, _difficulty);
}

void SnakeGame::set_difficulty(int d) {
    if (d < 0 || d > 2) d = 1;
    _difficulty = static_cast<Difficulty>(d);
    // Applies immediately (tick rate + wall behavior); score/snake are untouched.
    _apply_difficulty();
}

Vector2i SnakeGame::get_last_eaten_cell() const {
    return Vector2i(_lastEatenCell.x, _lastEatenCell.y);
}

void SnakeGame::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) return;
    set_process(true);
    queue_redraw();
}

Direction SnakeGame::_read_input() const {
    Input* in = Input::get_singleton();
    if (in->is_action_pressed("ui_up")) return Direction::Up;
    if (in->is_action_pressed("ui_down")) return Direction::Down;
    if (in->is_action_pressed("ui_left")) return Direction::Left;
    if (in->is_action_pressed("ui_right")) return Direction::Right;
    return Direction::None;
}

void SnakeGame::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) return;
    if (_state.gameOver) return;

    _accumulator += delta;

    // Fixed-timestep loop; guard prevents a spiral of death after a long stall.
    int guard = 0;
    while (_accumulator >= _state.tickRate && guard < 8) {
        _accumulator -= _state.tickRate;
        _on_tick();
        ++guard;
        if (_state.gameOver) break;
    }
}

void SnakeGame::_on_tick() {
    Vec2i foodBefore = _state.food.position();  // captured before tick may respawn it
    TickResult r = tick(_state, _read_input());

    if (r.foodEaten) {
        _lastEatenCell = foodBefore;
        emit_signal("food_eaten", _state.score);
    }
    if (r.gameOver) {
        emit_signal("game_over", _state.score);
    }
    queue_redraw();
}

void SnakeGame::_draw() {
    const int CELL = CELL_SIZE;

    // Food
    Vec2i f = _state.food.position();
    draw_rect(Rect2(f.x * CELL, f.y * CELL, CELL, CELL), Color(1.0, 0.2, 0.2));

    // Snake
    const auto& body = _state.snake.body();
    for (size_t i = 0; i < body.size(); ++i) {
        const Vec2i& p = body[i];
        Color c = (i == 0) ? Color(0.2, 1.0, 0.2) : Color(0.1, 0.8, 0.1);
        draw_rect(Rect2(p.x * CELL, p.y * CELL, CELL - 1, CELL - 1), c);
    }
}

}  // namespace snake