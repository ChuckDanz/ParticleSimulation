#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "solver.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>



// Draw quadtree node boundaries recursively
inline void renderQuadtree(sf::RenderTarget& target, int node_index)
{
    if (node_index < 0 || node_index >= nodes.size()) return;

    const Node& node = nodes[node_index];

    // Draw this node's boundary as a rectangle outline
    static sf::RectangleShape rect;  // Reuse to avoid allocations
    rect.setSize(sf::Vector2f(node.half_W * 2.0f, node.half_H * 2.0f));
    rect.setPosition(sf::Vector2f(node.x - node.half_W, node.y - node.half_H));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(1.0f);
    target.draw(rect);

    // Recurse into children
    for (int i = 0; i < 4; i++)
    {
        if (node.children[i] != -1)
        {
            renderQuadtree(target, node.children[i]);
        }
    }
}

// Draw spatial grid overlay
inline void renderGrid(sf::RenderTarget& target, int window_size, int gridsize)
{
    int num_cells = window_size / gridsize;
    
    sf::RectangleShape gridLine;
    gridLine.setFillColor(sf::Color(100, 100, 100, 100));  // Semi-transparent gray
    
    // Draw vertical lines
    for (int i = 0; i <= num_cells; i++)
    {
        float x = i * gridsize;
        gridLine.setSize(sf::Vector2f(1.0f, window_size));
        gridLine.setPosition(sf::Vector2f(x, 0));
        target.draw(gridLine);
    }
    
    // Draw horizontal lines
    for (int j = 0; j <= num_cells; j++)
    {
        float y = j * gridsize;
        gridLine.setSize(sf::Vector2f(window_size, 1.0f));
        gridLine.setPosition(sf::Vector2f(0, y));
        target.draw(gridLine);
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
    if (showQuadtree && root_index != -1)
    {
        renderQuadtree(target, root_index);  // Pass root index instead of pointer
    }
}

// Render with spatial grid overlay
inline void renderWithGrid(sf::RenderTarget& target, Solver& solver, int window_size, int gridsize, bool showGrid = true)
{
    // Draw particles first
    render(target, solver);
    
    // Draw grid overlay only if enabled
    if (showGrid)
    {
        renderGrid(target, window_size, gridsize);
    }
}



#endif