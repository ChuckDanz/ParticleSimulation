#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <iostream>
#include <vector>
#include "particle.hpp"

class Solver
{
private:
    std::vector<Particle> objects;
    static constexpr float dt = 1.0f / 60;
    static constexpr Vec2 gravity = Vec2(0.0f, 9.81f * 50.0f);

    void applyGravity();

    void updateObjects(float dt);

public:
    Solver() = default;

    Particle& addObject(Vec2 p_position, float radius);

    void update();

    const std::vector<Particle>& getObjects() const;


};

#endif