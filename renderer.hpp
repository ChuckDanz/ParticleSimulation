#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "solver.hpp"
#include "thread.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class Renderer
{
public:
    
    Renderer(sf::RenderWindow& target_, Solver& solver_, Threader& threader_)
        : target{target_}, solver{solver_}, threader{threader_}
    {
        if (!obj_texture.loadFromFile("/mnt/c/Projects/ParticleSimulation/circle.png"))
        {
            std::cout << "Failed to load texture\n";
        }
        obj_texture.generateMipmap();
        obj_texture.setSmooth(true);
    }

    void newRender() {
        target.clear(sf::Color::Black);
        updateVA();

        //target.draw(box_va);
        //target.draw(trail_va);
        
        sf::RenderStates states;
        states.texture = &obj_texture;
        //target.draw(dot_va, states);
        target.draw(obj_va, states);
    }

    void updateVA() {
        obj_va.resize(solver.objects.size() * 6);
        const float tex_size = 1024.0f;
        const float radius = solver.objects[0].m_radius;
        
        threader.parallel(solver.objects.size(), [&](int start, int end) {
            for (int i = start; i < end; i++) {
                const Particle& obj = solver.objects[i];
                const int id = i * 6;

                const sf::Color color = obj.getColor();
            
                // Positions for quad corners
                sf::Vector2f tl(obj.m_position.x - radius, obj.m_position.y - radius);
                sf::Vector2f tr(obj.m_position.x + radius, obj.m_position.y - radius);
                sf::Vector2f br(obj.m_position.x + radius, obj.m_position.y + radius);
                sf::Vector2f bl(obj.m_position.x - radius, obj.m_position.y + radius);
                
                // First triangle (top-left, top-right, bottom-right)
                obj_va[id    ].position = tl;
                obj_va[id + 1].position = tr;
                obj_va[id + 2].position = br;
                
                obj_va[id    ].texCoords = {0.0f, 0.0f};
                obj_va[id + 1].texCoords = {tex_size, 0.0f};
                obj_va[id + 2].texCoords = {tex_size, tex_size};
                
                obj_va[id    ].color = color;
                obj_va[id + 1].color = color;
                obj_va[id + 2].color = color;
                
                // Second triangle (top-left, bottom-right, bottom-left)
                obj_va[id + 3].position = tl;
                obj_va[id + 4].position = br;
                obj_va[id + 5].position = bl;
                
                obj_va[id + 3].texCoords = {0.0f, 0.0f};
                obj_va[id + 4].texCoords = {tex_size, tex_size};
                obj_va[id + 5].texCoords = {0.0f, tex_size};
                
                obj_va[id + 3].color = color;
                obj_va[id + 4].color = color;
                obj_va[id + 5].color = color;

                
            }
        });

        // dot_va.resize(solver.dot_obstacles.size() * 4);
        // for (int i = 0; i < solver.dot_obstacles.size(); i++) {
        //     const int id = i * 4;
        //     const ObstacleDot& obj = solver.dot_obstacles[i];
        //     const float obj_rad = obj.radius;
        //     sf::Color   color   = obj.color;
        //     dot_va[id    ].position = obj.position + sf::Vector2f{-obj_rad, -obj_rad};
        //     dot_va[id + 1].position = obj.position + sf::Vector2f{ obj_rad, -obj_rad};
        //     dot_va[id + 2].position = obj.position + sf::Vector2f{ obj_rad,  obj_rad};
        //     dot_va[id + 3].position = obj.position + sf::Vector2f{-obj_rad,  obj_rad}; 

        //     dot_va[id    ].texCoords = {0.0f, 0.0f};
        //     dot_va[id + 1].texCoords = {tex_size, 0.0f};
        //     dot_va[id + 2].texCoords = {tex_size, tex_size};
        //     dot_va[id + 3].texCoords = {0.0f, tex_size};
            
        //     dot_va[id    ].color = color;
        //     dot_va[id + 1].color = color;
        //     dot_va[id + 2].color = color;
        //     dot_va[id + 3].color = color;
        // }

        // box_va.resize(solver.box_obstacles.size() * 4);
        // for (int i = 0; i < solver.box_obstacles.size(); i++) {
        //     const int id = i * 4;
        //     const ObstacleBox& obj = solver.box_obstacles[i];
        //     const sf::Vector2f size = obj.dimensions * 0.5f;
        //     sf::Color         color = obj.color;
        //     sf::Transform rotation;
        //     rotation.rotate(obj.rotation);
        //     box_va[id    ].position = obj.position + rotation.transformPoint(-size.x, -size.y);
        //     box_va[id + 1].position = obj.position + rotation.transformPoint( size.x, -size.y);
        //     box_va[id + 2].position = obj.position + rotation.transformPoint( size.x,  size.y);
        //     box_va[id + 3].position = obj.position + rotation.transformPoint(-size.x,  size.y);

        //     if (obj.breakable) color.a = obj.durability * 255 / obj.total_dur;
        //     box_va[id    ].color = color;
        //     box_va[id + 1].color = color;
        //     box_va[id + 2].color = color;
        //     box_va[id + 3].color = color;
        // }
    }

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
        //render(target, solver);
        newRender();
        // Draw grid overlay only if enabled
        if (showGrid)
        {
            renderGrid(target, window_size, gridsize);
        }
    }
private:
    sf::RenderWindow& target;
    Solver& solver;
    Threader& threader;
    sf::Texture     obj_texture;
    sf::VertexArray obj_va{sf::PrimitiveType::Triangles};

};


#endif
