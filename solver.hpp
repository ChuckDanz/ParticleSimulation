#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <iostream>
#include <vector>
#include <array>
#include "particle.hpp"

class Solver
{
private:
    std::vector<Particle> objects;
    static constexpr float dt = 1.0f / 60;
    static constexpr Vec2 gravity = Vec2(0.0f, 9.81f * 50.0f);

    static constexpr int substeps = 8; 

    static constexpr float window_size = 800.0f;

    Vec2 boundary_center = Vec2{420.0f, 420.0f};
    float boundary_radius = 100.0f;

    float boundary_attributes[3] = {0.0f, 0.0f, 0.0f}; // x, y, radius

    void applyGravity();

    void updateObjects(float dt);

    Vec2 calculateBounceBack(const Vec2& p_velocity, const Vec2& p_normal_col);


public:
    Solver() = default;

    Particle& addObject(const Vec2& p_position, float radius);

    void update();

    const std::vector<Particle>& getObjects() const;

    // for a circle
    void applyBoundary();

    //this is for the borders of the window
    void applyBorder();

    std::array<float, 3> getBoundary() const;

    void setBoundary(const Vec2& position, float radius);

    void mousePull(const Vec2& position);

    void mousePush(const Vec2& position);

    void setObjectVelocity(Particle& particle, Vec2 v);

    void checkCollisions();
    


};

#endif