#include <SFML/Window.hpp>
#include "renderer.hpp"

int main()
{

    constexpr uint32_t window_width = 800;
    constexpr uint32_t window_height = 600;


    sf::RenderWindow window(sf::VideoMode({window_width, window_height}), "My window");


    //window.setPosition(sf::Vector2i(-1000, 1500));

    const uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);

    // run the program as long as the window is open

    Solver solver;
    

    solver.setBoundary(Vec2{window_width / 2.0f, window_height / 2.0f}, (window_width - 250.0f) / 2.0f);

    const std::array<float, 3> boundary = solver.getBoundary();
    sf::CircleShape boundary_background{boundary[2]};
    boundary_background.setOrigin(sf::Vector2(boundary[2], boundary[2]));
    boundary_background.setFillColor(sf::Color::Black);
    boundary_background.setPosition(sf::Vector2(boundary[0], boundary[1]));
    boundary_background.setPointCount(128);

    auto& object = solver.addObject(Vec2({420.0f, 100.0f}), 10.0f);


    while (window.isOpen()) // this is where we will update 
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            float ratio = 840.0f / window.getSize().x;
            sf::Vector2f pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * ratio;
            solver.mousePull(Vec2(pos.x, pos.y));
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        {
            float ratio = 840.0f / window.getSize().x;
            sf::Vector2f pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * ratio;
            solver.mousePush(Vec2(pos.x, pos.y));
        }
        solver.update();
        window.clear(sf::Color::White);
        window.draw(boundary_background);
        render(window, solver);
        window.display();
    }

    return 0;
}