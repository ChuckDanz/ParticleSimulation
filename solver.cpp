#include "solver.hpp"

Particle& Solver::addObject(const Vec2& p_position, float radius)
{
    return objects.emplace_back(Particle(p_position, radius));
}

void Solver::update()
{
    float substep_dt = dt / substeps;
    for (int i = 0; i < substeps; i++)
    {    
        applyGravity();
        applyBoundary();   
        updateObjects(substep_dt);  
    }
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

void Solver::applyBoundary()
{
    for (auto &particle : objects)
    {
        const Vec2 r = boundary_center - particle.m_position; 
        const float dist = sqrt(r.x * r.x + r.y * r.y); 


        if (dist > boundary_radius - particle.m_radius)
        {
            const Vec2 normal_v = r / dist;
            const Vec2 perp = {-normal_v.y, normal_v.x};
            const Vec2 velocity = particle.getVelocity();
            particle.m_position = boundary_center - normal_v * (boundary_radius - particle.m_radius);
            particle.setVelocity(calculateBounceBack(velocity, perp), 1.0f);
            
        }
    }
}

Vec2 Solver::calculateBounceBack(const Vec2& p_velocity, const Vec2& p_normal_col)
{
    return 2.0f * p_velocity.dot(p_normal_col) * p_normal_col - p_velocity;
}

std::array<float, 3> Solver::getBoundary() const
{
    return {boundary_center.x, boundary_center.y, boundary_radius};
}

void Solver::setBoundary(const Vec2& position, float radius)
{
    boundary_center = position;
    boundary_radius = radius;

    boundary_attributes[0] = position.x;
    boundary_attributes[1] = position.y;
    boundary_attributes[2] = radius;
}

void Solver::mousePull(const Vec2& position)
{
    for (auto &particle : objects)
    {
        Vec2 dir = position - particle.m_position;
        float distance = sqrt(dir.x * dir.x + dir.y * dir.y);
        particle.accelerate(dir * std::max(0.0f, 10 * (120 - distance)));

    }
}

void Solver::mousePush(const Vec2& position)
{
    for (auto &particle : objects)
    {
        Vec2 dir = position - particle.m_position;
        float distance = sqrt(dir.x * dir.x + dir.y * dir.y);
        particle.accelerate(dir * std::min(0.0f, -10 * (120 - distance)));
    }
}