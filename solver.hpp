#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <iostream>
#include <vector>
#include <array>
#include <deque>
#include <algorithm>
#include "particle.hpp"
#include "quadtree.hpp"


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
    void updateObjectsGrid(float dt);

    Vec2 calculateBounceBack(const Vec2& p_velocity, const Vec2& p_normal_col);
    
    void spatialSort();

    uint64_t mortonEncode(uint32_t x, uint32_t y);

    void computeCollision(Particle* p_1, Particle* p_2);
    void collideCells(int x1, int y1, int x2, int y2);

public:
    Solver()
    {
        objects.reserve(3000);
    }

    static constexpr int gridsize = 10;

    Particle& addObject(const Vec2& p_position, float radius);
    Particle& addObjectGrid(const Vec2& p_position, float radius);

    std::vector<int> grid[350][350];

    void updateQuadtree();
    void updateGrid();

    const std::vector<Particle>& getObjects() const;

    // for a circle
    void applyBoundary();

    //this is for the borders of the window
    void applyBorder();

    void updateTree();

    std::array<float, 3> getBoundary() const;

    void setBoundary(const Vec2& position, float radius);

    void mousePull(const Vec2& position);

    void mousePush(const Vec2& position);

    void setObjectVelocity(Particle& particle, Vec2 v);

    void checkCollisions(std::vector<std::pair<Particle*, Particle*>>& collision_pairs);
    void checkCollisionsGrid();
    


};

#endif
