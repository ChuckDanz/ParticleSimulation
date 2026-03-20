#ifndef VEC2_HPP
#define VEC2_HPP

#include <cmath>

struct Vec2
{
    float x{};
    float y{};

    constexpr Vec2(float x_input = 0.0f, float y_input = 0.0f) : x(x_input), y(y_input) {}
    constexpr Vec2(const Vec2& pos_vector) : x(pos_vector.x), y(pos_vector.y) {}

    inline Vec2 operator+(const Vec2& other_vector) const {
        return Vec2(x + other_vector.x, y + other_vector.y);
    }

    inline Vec2& operator+=(const Vec2& other_vector) {
        x += other_vector.x;
        y += other_vector.y;
        return *this;
    }

    inline Vec2 operator-(const Vec2& other_vector) const {
        return Vec2(x - other_vector.x, y - other_vector.y);
    }

    inline Vec2& operator-=(const Vec2& other_vector) {
        x -= other_vector.x;
        y -= other_vector.y;
        return *this;
    }

    inline Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    friend inline Vec2 operator*(float scalar, const Vec2& vector) {
        return Vec2(vector.x * scalar, vector.y * scalar);
    }

    inline Vec2 operator/(float scalar) const {
        return Vec2(x / scalar, y / scalar);
    }

    inline float dot(const Vec2& other_vector) const {
        return x * other_vector.x + y * other_vector.y;
    }

    inline float magnitude() const {
        return std::sqrt((x * x) + (y * y));
    }

    inline Vec2 normalize() const {
        float mag = magnitude();
        if (mag == 0.0f) return Vec2{0.0f, 0.0f};
        return Vec2{x / mag, y / mag};
    }
};

#endif
