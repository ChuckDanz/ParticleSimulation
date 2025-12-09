#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "solver.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>



// Draw quadtree node boundaries recursively
inline void renderQuadtree(sf::RenderTarget& target, Node* node)
{
    if (!node) return;

    // Draw this node's boundary as a rectangle outline
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(node->half_W * 2.0f, node->half_H * 2.0f));
    rect.setPosition(sf::Vector2f(node->x - node->half_W, node->y - node->half_H));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(1.0f);
    target.draw(rect);

    // Optionally show particle count in this node
    // (requires sf::Font setup — skip if you don't need it)

    // Recurse into children
    for (const auto& child : node->children)
    {
        if (child)
        {
            renderQuadtree(target, child.get());
        }
    }
}


inline void render(sf::RenderTarget& target, Solver& solver) 
{

    //target.clear(sf::Color::Black);
    sf::CircleShape circle{1.0f};
    circle.setPointCount(32);
    circle.setOrigin(sf::Vector2f(1.0f, 1.0f));
    
    const auto& objects = solver.getObjects();
    for (const auto& particle : objects)
    {
        circle.setPosition(sf::Vector2f(particle.m_position.x, particle.m_position.y));
        circle.setScale(sf::Vector2f(particle.m_radius, particle.m_radius));
        circle.setFillColor(particle.getColor());
        target.draw(circle);
    }
};

// Combined render with debug overlay
inline void renderWithDebug(sf::RenderTarget& target, Solver& solver, bool showQuadtree = true)
{
    // Draw particles first
    render(target, solver);

    // Draw quadtree overlay
    if (showQuadtree && root)
    {
        renderQuadtree(target, root.get());
    }
}




#endif