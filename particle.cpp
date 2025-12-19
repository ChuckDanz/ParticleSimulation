#include "particle.hpp"

Particle::Particle() = default;

Particle::Particle(const Vec2& p_position, float p_radius)
    : m_position{p_position},
    m_position_last{p_position},
    m_acceleration{10.0f, 10.0f},
    index{0},
    m_radius{p_radius}
{}

void Particle::update(float dt)
{
    Vec2 displacement = m_position - m_position_last;
    m_position_last = m_position;
    m_position = m_position + displacement + m_acceleration * (dt * dt);
    m_acceleration = {0.0f, 0.0f}; //reset acceleration
}

void Particle::accelerate(const Vec2& p_acceleration)
{
    m_acceleration += p_acceleration;
}

void Particle::setVelocity(const Vec2& p_velocity, float dt)
{
    m_position_last = m_position - (p_velocity * dt);
}

void Particle::addVelocity(const Vec2& p_velocity, float dt)
{
    m_position_last -= p_velocity * dt;
}

Vec2 Particle::getVelocity()
{
    return m_position - m_position_last;
}

void Particle::setColor(sf::Color color)
{
    m_color = color;
}

sf::Color Particle::getColor() const
{
    return m_color;
}

