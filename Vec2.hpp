#ifndef VEC2_HPP
#define VEC2_HPP

#include <cmath>

struct Vec2
{
    float x{};
    float y{};

    constexpr Vec2(float x_input = 0.0f, float y_input = 0.0f) : x(x_input), y(y_input) {};

    constexpr Vec2(const Vec2& pos_vector) : x(pos_vector.x), y(pos_vector.y) {};

    Vec2 operator+(const Vec2& other_vector) const;

    Vec2& operator+=(const Vec2& other_vector);

    Vec2 operator-(const Vec2& other_vector) const; //const on outsude doesnt modify "this" object

    Vec2& operator-=(const Vec2& other_vector);

    Vec2 operator*(const float scalar) const;
    friend Vec2 operator*(float scalar, const Vec2& vector);

    Vec2 operator/(const float scalar) const;

    float dot(const Vec2& other_vector) const;

    float magnitude() const;

    Vec2 normalize() const;
};
    

#endif