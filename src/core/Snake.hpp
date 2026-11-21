#pragma once
#include "Types.hpp"
#include <deque>

class Snake {
    std::deque<Vec2i> body_;
    Direction dir_ = Direction::Right;
    bool growPending_ = false;

public:
    Vec2i head() const { return body_.front(); }
    const std::deque<Vec2i>& body() const { return body_; }
    size_t size() const { return body_.size(); }
    Direction direction() const { return dir_; }

    void setDirection(Direction d) {
        if (d != Direction::None) dir_ = d;
    }

    // Places the head at `start` with `length` segments trailing behind `dir`.
    void init(const Vec2i& start, int length, Direction dir);

    // Advances the head one cell forward; pops the tail unless a grow is pending.
    void move();

    // Wraps the head position around a grid of size w x h.
    void wrapHead(int w, int h);

    void grow() { growPending_ = true; }

    bool occupies(const Vec2i& pos) const;
};