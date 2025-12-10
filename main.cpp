#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include "renderer.hpp"

static sf::Color getColor(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * M_PI);
    const float b = sin(t + 0.66f * 2.0f * M_PI);
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}

int main()
{

    float max_angle = 120.0f * M_PI / 180.0f;

    constexpr float spawn_velocity = 0.5f;

    constexpr uint32_t window_width = 800;
    constexpr uint32_t window_height = 800;

    constexpr uint32_t max_objects = 2000;
    constexpr float spawn_delay = 0.01f;


    sf::RenderWindow window(sf::VideoMode({window_width, window_height}), "My window");

    sf::Clock globalClock;
    sf::Clock clock;

    sf::Clock respawnClock, timer, fpstimer;
    sf::Font arialFont;
    arialFont.openFromFile("/mnt/c/Projects/ParticleSimulation/arial.ttf");

    if (!arialFont.openFromFile("/mnt/c/Projects/ParticleSimulation/arial.ttf"))
    {
        std::cout << "Failed to load font\n";
        return -1; // error
    }


    //window.setPosition(sf::Vector2i(-1000, 1500));

    const uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);

    // run the program as long as the window is open

    Solver solver;
    

    // circular boundary stuff
    //solver.setBoundary(Vec2{window_width / 2.0f, window_height / 2.0f}, (window_width - 250.0f) / 2.0f);

    // const std::array<float, 3> boundary = solver.getBoundary();
    // sf::CircleShape boundary_background{boundary[2]};
    // boundary_background.setOrigin(sf::Vector2(boundary[2], boundary[2]));
    // boundary_background.setFillColor(sf::Color::Black);
    // boundary_background.setPosition(sf::Vector2(boundary[0], boundary[1]));
    // boundary_background.setPointCount(128);

    //auto& object = solver.addObject(Vec2{420.0f, 100.0f}, 10.0f);


    while (window.isOpen()) // this is where we will update 
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        if (solver.getObjects().size() < max_objects && clock.getElapsedTime().asSeconds() >= spawn_delay)
        {
            float t = globalClock.getElapsedTime().asSeconds();
            auto& particle = solver.addObject(Vec2{420.0f, 100.0f}, 3.0f);
            
            float angle = M_PI * 0.5f + max_angle * sin(3.0f * t);

            particle.setColor(getColor(t));
            
            solver.setObjectVelocity(particle, spawn_velocity * Vec2{cos(angle), sin(angle)});
            clock.restart();
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            float ratio = 840.0f / window.getSize().x;
            sf::Vector2f pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * ratio;
            solver.mousePull(Vec2{pos.x, pos.y});
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        {
            float ratio = 840.0f / window.getSize().x;
            sf::Vector2f pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * ratio;
            solver.mousePush(Vec2{pos.x, pos.y});
        }

        fpstimer.restart();
        solver.update();
        float solver_ms = fpstimer.getElapsedTime().asMicroseconds() / 1000.0f;
        
        fpstimer.restart();
        window.clear(sf::Color::White);
        renderWithDebug(window, solver, false);
        float render_ms = fpstimer.getElapsedTime().asMicroseconds() / 1000.0f;

        sf::Text number(arialFont);
        number.setFont(arialFont);
        number.setString("Solver: " + std::to_string(solver_ms) + "ms | Render: " + std::to_string(render_ms) + 
                        "ms | Total: " + std::to_string(solver_ms + render_ms) + "ms | " + 
                        std::to_string(solver.getObjects().size()) + " particles");
        number.setCharacterSize(20);
        number.setFillColor(sf::Color::Magenta);
        window.draw(number);

        window.display();
    }

    return 0;
}

