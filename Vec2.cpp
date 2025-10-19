#include "Vec2.hpp"


Vec2 Vec2::operator+(const Vec2& other_vector) const 
{
    return Vec2(x + other_vector.x, y + other_vector.y);
}

Vec2& Vec2::operator+=(const Vec2& other_vector) 
{
    x += other_vector.x;
    y += other_vector.y;
    return *this;
}

Vec2 Vec2::operator-(const Vec2& other_vector) const //const on outsude doesnt modify "this" object
{
    return Vec2(x - other_vector.x, y - other_vector.y);
}

Vec2& Vec2::operator-=(const Vec2& other_vector) 
{
    x -= other_vector.x;
    y -= other_vector.y;
    return *this;
}

Vec2 Vec2::operator*(const float scalar) const 
{
    return Vec2(x * scalar, y * scalar);
}

Vec2 operator*(float scalar, const Vec2& vector)
{
    return Vec2(vector.x * scalar, vector.y * scalar);
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

Vec2 Vec2::normalize() const
{
    float mag = magnitude();
    if (mag == 0.0f) // check for divison by 0
    {
        return Vec2{0.0f, 0.0f};
    }
    return Vec2{x / mag, y / mag};
}





