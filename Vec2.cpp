#include "Vec2.hpp"


Vec2::Vec2(): x{0.0f}, y{0.0f} {}

Vec2::Vec2(float x_input, float y_input) : x{x_input} , y{y_input} {}

Vec2 Vec2::operator+(const Vec2& other_vector) const 
{
    return Vec2(x + other_vector.x, y + other_vector.y);
}

Vec2 Vec2::operator-(const Vec2& other_vector) const //const on outsude doesnt modify "this" object
{
    return Vec2(x - other_vector.x, y - other_vector.y);
}

Vec2 Vec2::operator*(const float scalar) const 
{
    return Vec2(x * scalar, y * scalar);
}

Vec2 Vec2::operator/(const float scalar) const
{
    return Vec2(x / scalar, y / scalar);
}

float Vec2::dot(const Vec2& other_vector) const
{
    return ((x * other_vector.x) + (y * other_vector.y));
}

float Vec2::magnitude() const
{
    return std::sqrt((x * x) + (y * y));
}





