cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add compiler optimizations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set optimization flags
if(MSVC)
    # Visual Studio compiler
    add_compile_options(/O2 /Oi /Ot /GL)
    add_link_options(/LTCG)
else()
    # GCC/Clang compiler
    add_compile_options(-O3 -march=native -ffast-math)
endif()

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main main.cpp particle.cpp solver.cpp Vec2.hpp renderer.hpp quadtree.cpp quadtree.hpp thread.hpp grid.hpp ESolver.hpp)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics)
