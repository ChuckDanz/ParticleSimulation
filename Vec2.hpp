#ifndef VEC2_HPP
#define VEC2_HPP

#include <cmath>

struct Vec2
{
    float x;
    float y;

    Vec2();
    Vec2(float x_input, float y_input);

    Vec2 operator+(const Vec2& other_vector) const;

    Vec2 operator-(const Vec2& other_vector) const; //const on outsude doesnt modify "this" object

    Vec2 operator*(const float scalar) const;

    Vec2 operator/(const float scalar) const;

    float dot(const Vec2& other_vector) const;

    float magnitude() const;
};
    

#endif