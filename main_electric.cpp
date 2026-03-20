#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include "Esolver.hpp"
#include "ERenderer.hpp"

int main()
{
    constexpr int W = 600;
    constexpr int H = 600;

    sf::RenderWindow window(sf::VideoMode({(unsigned)W, (unsigned)H}), "Lichtenberg Figures");
    window.setFramerateLimit(60);

    ESolver esolver(W, H);
    esolver.setGrid();

    ERenderer erenderer(W, H);

    sf::Font font;
    bool hasFont = font.openFromFile("/mnt/c/Projects/ParticleSimulation/arial.ttf");

    sf::Clock fpsClock;
    float fps = 0.0f;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    esolver.setGrid();
                    esolver.active.clear(); // clear stale pointers before re-zapping
                    esolver.zap(mouse->position.y, mouse->position.x);
                }
            }
        }

        if (!esolver.active.empty())
            esolver.update();

        fps = 1.0f / fpsClock.restart().asSeconds();

        window.clear(sf::Color::Black);
        erenderer.render(window, esolver);

        if (hasFont)
        {
            sf::Text text(font);
            text.setString("FPS: " + std::to_string((int)fps) + "  |  Left-click to reset");
            text.setCharacterSize(14);
            text.setFillColor(sf::Color(160, 160, 180));
            window.draw(text);
        }

        window.display();
    }

    return 0;
}
