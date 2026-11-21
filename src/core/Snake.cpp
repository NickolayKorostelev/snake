#include "Snake.hpp"

void Snake::init(const Vec2i& start, int length, Direction dir) {
    body_.clear();
    dir_ = dir;
    growPending_ = false;
    if (length <= 0) return;

    // Body trails behind the head, i.e. in the opposite direction of travel.
    Vec2i back = dirVector(opposite(dir));
    for (int i = 0; i < length; ++i) {
        body_.push_back({ start.x + back.x * i, start.y + back.y * i });
    }
}

void Snake::move() {
    if (body_.empty()) return;
    Vec2i step = dirVector(dir_);
    Vec2i newHead = { body_.front().x + step.x, body_.front().y + step.y };
    body_.push_front(newHead);
    if (growPending_) {
        growPending_ = false;
    } else {
        body_.pop_back();
    }
}

void Snake::wrapHead(int w, int h) {
    if (body_.empty()) return;
    Vec2i& hd = body_.front();
    if (hd.x < 0) hd.x = w - 1;
    if (hd.x >= w) hd.x = 0;
    if (hd.y < 0) hd.y = h - 1;
    if (hd.y >= h) hd.y = 0;
}

bool Snake::occupies(const Vec2i& pos) const {
    for (const auto& c : body_) {
        if (c == pos) return true;
    }
    return false;
}