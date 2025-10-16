#include "solver.hpp"

Particle& Solver::addObject(Vec2 p_position, float radius)
{
    return objects.emplace_back(Particle(p_position, radius));
}

void Solver::update()
{
    applyGravity();
    updateObjects(dt);
}

void Solver::applyGravity()
{
    for (auto &particle : objects)
        particle.accelerate(gravity);
}

void Solver::updateObjects(float dt)
{
    for (auto &particle : objects)
        particle.update(dt);
}

const std::vector<Particle>& Solver::getObjects() const
{
    return objects;
}