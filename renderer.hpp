#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "solver.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

inline void render(sf::RenderTarget& target, Solver& solver) 
{
    sf::CircleShape circle{1.0f};
    circle.setPointCount(32);
    const auto& objects = solver.getObjects();
    for (const auto& particle : objects)
    {
        circle.setPosition(sf::Vector2f(particle.m_position.x, particle.m_position.y));
        circle.setScale(sf::Vector2f(particle.m_radius, particle.m_radius));
        circle.setFillColor(sf::Color::Red);
        target.draw(circle);
    }
};


#endif