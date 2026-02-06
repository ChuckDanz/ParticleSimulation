#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <string>
#include <fstream>
#include <unordered_map>
#include <filesystem>
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

void saveParticles(const std::vector<Particle>& objects, const std::string& filename)
{
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    
    // Write header (optional, helps Python parse)
    file << objects.size() << "\n";
    
    // Write each particle: id, x, y, r, g, b
    for (const Particle& p : objects)
    {
        sf::Color c = p.getColor();
        file << p.id << " " 
             << p.m_position.x << " " 
             << p.m_position.y << " "
             << (int)c.r << " " 
             << (int)c.g << " " 
             << (int)c.b << "\n";
    }
    
    file.close();
    std::cout << "Saved " << objects.size() << " particles to " << filename << "\n";
}


int main()
{

    float max_angle = 120.0f * M_PI / 180.0f;

    constexpr float spawn_velocity = 0.5f;

    constexpr uint32_t window_width = 800;
    constexpr uint32_t window_height = 800;

    constexpr uint32_t max_objects = 53000;
    constexpr float spawn_delay = 0.01f; //0.1f

    constexpr float RADIUS = 2.0f;
    
    const std::string COLLISION_TYPE = "GRID"; //GRID or QUADTREE 
    
    unsigned int SPAWNPOINTS = 10;

    bool showDebugGrid = true;  // Toggle grid visualization
    bool showCounts = false;     // Toggle particle count display

	bool recordPositions = true;


    sf::RenderWindow window(sf::VideoMode({window_width, window_height}), "My window");

    int window_x = window_width / 2;
    int window_y = window_height / 2;
    window.setPosition(sf::Vector2i(window_x, window_y));

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
    unsigned int worker_threads = std::max(1u, std::thread::hardware_concurrency() - 2);
    std::cout << "Worker Threads: " << worker_threads;
    Threader threadPool(10);
    Solver solver(window_width, window_height, RADIUS, threadPool);
    Renderer renderer(window, solver, threadPool);

    std::unordered_map<int, sf::Color> loadedColors;  // Dictionary: particle ID → color

    std::string colorFilePath = "./Vector.txt";
    bool useCustomColors = std::filesystem::exists(colorFilePath);
    std::cout << "\nUsing custom colors: " << (useCustomColors ? "YES" : "NO") << std::endl;

    // Actually load the colors into the map!
    if (useCustomColors)
    {
        std::ifstream colorFile(colorFilePath);
        int id, r, g, b;
        while (colorFile >> id >> r >> g >> b)
        {
            loadedColors[id] = sf::Color(r, g, b);
        }
        colorFile.close();
        std::cout << "Loaded " << loadedColors.size() << " colors from colors.txt\n";
    }


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
                
            // Handle key presses for debug toggles
            if (event->is<sf::Event::KeyPressed>())
            {
                const auto& keyEvent = event->getIf<sf::Event::KeyPressed>();
                
                // Toggle grid visualization with 'G'
                if (keyEvent->code == sf::Keyboard::Key::G)
                {
                    showDebugGrid = !showDebugGrid;
                    std::cout << "Debug grid: " << (showDebugGrid ? "ON" : "OFF") << std::endl;
                }
                
                // Toggle particle count display with 'C'
                if (keyEvent->code == sf::Keyboard::Key::C)
                {
                    showCounts = !showCounts;
                    std::cout << "Particle counts: " << (showCounts ? "ON" : "OFF") << std::endl;
                }
            }
        }

        if (solver.getObjects().size() < max_objects && clock.getElapsedTime().asSeconds() >= spawn_delay)
        {
            float t = globalClock.getElapsedTime().asSeconds();
            Particle* particle = nullptr;

            if (COLLISION_TYPE == "QUADTREE")
            {
                particle = &solver.addObject(Vec2{420.0f, 100.0f}, RADIUS);
                float angle = M_PI * 0.5f + max_angle * sin(3.0f); // (* t in sin) for variation over time

                particle->setColor(getColor(t));
                    
                solver.setObjectVelocity(*particle, spawn_velocity * Vec2{cos(angle), sin(angle)});

            }
            else if (COLLISION_TYPE == "GRID") 
            {
                for (int i = 0; i < SPAWNPOINTS; i++)
                {

                    particle = &solver.addObjectGrid(Vec2{420.0f, 100.0f + 40.0f * i}, RADIUS);
                    float angle = M_PI * 0.5f + max_angle * sin(3.0f); // (* t in sin) for variation over time

                    if (useCustomColors && loadedColors.count(particle->id)) {
                        particle->setColor(loadedColors[particle->id]);
                    } else {
                        particle->setColor(getColor(t));
                    }
                    

                    solver.setObjectVelocity(*particle, spawn_velocity * Vec2{cos(angle), sin(angle)});
                        
                } 
            
            }

    

            clock.restart();
        }
        
        if (recordPositions && solver.getObjects().size() >= max_objects)
        {
            saveParticles(solver.getObjects(), "./particles2.txt");
            recordPositions = false;
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
    
        if (COLLISION_TYPE == "QUADTREE")
        {
            fpstimer.restart();
            solver.updateQuadtree();
            float solver_ms = fpstimer.getElapsedTime().asMicroseconds() / 1000.0f;
            
            fpstimer.restart();
            window.clear(sf::Color::White);
            renderer.renderWithDebug(window, solver, showDebugGrid);
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
        else if (COLLISION_TYPE == "GRID")
        {
            fpstimer.restart();
            solver.updateGrid();
            float solver_ms = fpstimer.getElapsedTime().asMicroseconds() / 1000.0f;
            
            fpstimer.restart();
            window.clear(sf::Color::White);
            
            // Render with grid debug overlay
            if (showCounts)
            {
                renderer.renderWithGrid(window, solver, window_width, solver.gridsize, showDebugGrid);
            }
            else
            {
                renderer.renderWithGrid(window, solver, window_width, solver.gridsize, showDebugGrid);
            }
            
            float render_ms = fpstimer.getElapsedTime().asMicroseconds() / 1000.0f;

            sf::Text number(arialFont);
            number.setFont(arialFont);
            number.setString("Solver: " + std::to_string(solver_ms) + "ms | Render: " + std::to_string(render_ms) + 
                    "ms | Total: " + std::to_string(solver_ms + render_ms) + "ms | " + 
                    std::to_string(solver.getObjects().size()) + " particles | Grid: " +
                    std::to_string(window_width / solver.gridsize) + "x" + std::to_string(window_height / solver.gridsize) +
                    " [G=Grid C=Counts]");
            number.setCharacterSize(20);
            number.setFillColor(sf::Color::Magenta);
            window.draw(number);

            window.display();
        }
    } // end of while (window.isOpen())	

    return 0;
}



