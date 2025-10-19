#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "solver.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

inline void render(sf::RenderTarget& target, Solver& solver) 
{
    const auto& objects = solver.getObjects();
    for (const auto& particle : objects)
    {
        sf::CircleShape circle{particle.m_radius};
        circle.setPointCount(32);
        circle.setOrigin(sf::Vector2f(particle.m_radius, particle.m_radius));
        circle.setPosition(sf::Vector2f(particle.m_position.x, particle.m_position.y));
        circle.setFillColor(sf::Color::Red);
        target.draw(circle);
    }
};


#endif