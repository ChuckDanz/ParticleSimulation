cmake_minimum_required(VERSION 3.15)
project(particlesim)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED) #find these components from sfml or throw error

add_executable(particlesim main.cpp particle.cpp Vec2.cpp)

target_link_libraries(particlesim PRIVATE SFML::Network SFML::Graphics SFML::Window SFML::Audio SFML::System)

