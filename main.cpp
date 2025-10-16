#include <SFML/Window.hpp>
#include "renderer.hpp"

int main()
{


    sf::RenderWindow window(sf::VideoMode({800, 600}), "My window");


    //window.setPosition(sf::Vector2i(-1000, 1500));

    const uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);

    // run the program as long as the window is open

    Solver solver;
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
        solver.update();
        window.clear(sf::Color::White);
        render(window, solver);
        window.display();
    }
}