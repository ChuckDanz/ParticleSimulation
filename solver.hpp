#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <iostream>
#include <vector>
#include <array>
#include <deque>
#include <algorithm>
#include "particle.hpp"
#include "quadtree.hpp"
#include "thread.hpp"


class Solver
{
private:

   // MAIN PROBLEM IS THAT WHEN THERE ARE TOO MANY PARTICLES ON TOP OF EACH OTHER THE BOTTOM ONES MERGE WITH EACH OTHER WE NEED SOME WAY TO DAMPEN THE 

    static constexpr float dt = 1.0f / 60;
    static constexpr Vec2 gravity = Vec2(0.0f, 9.81f * 25.0f);

    static constexpr int substeps = 8; 

    float window_size = 800.0f;
    float window_width = 800.0f;
    float window_height = 800.0f;
 
    Threader& threader;
  

    Vec2 boundary_center = Vec2{420.0f, 420.0f};
    float boundary_radius = 100.0f;

    float boundary_attributes[3] = {0.0f, 0.0f, 0.0f}; // x, y, radius

    void applyGravity();
    void updateGravityThreaded(int start, int end);

    void updateObjects(float dt);
    void updateObjectsGrid(float dt);

    Vec2 calculateBounceBack(const Vec2& p_velocity, const Vec2& p_normal_col);
    
    void spatialSort();
    void spatialSortGrid();

    uint64_t mortonEncode(uint32_t x, uint32_t y);

    void computeCollision(Particle* p_1, Particle* p_2);
    void collideCells(int x1, int y1, int x2, int y2);
    
    int num_cells_height = 0;
    int num_cells_width = 0;
public:
    Solver(float width, float height, float radius, Threader& threader_) :
	    window_width{width},
	    window_height{height},
        window_size{std::min(width, height)},
	    gridsize{static_cast<int>(radius * 2.0f)}, //radius * 2.0f
	    threader{threader_},
	    num_cells_width{static_cast<int>(window_width / (radius * 2.0f))},
	    num_cells_height{static_cast<int>(window_height / (radius * 2.0f))}
    {

    	objects.reserve(3000);
        int num_cells = num_cells_width * num_cells_height;
        cell_counts.resize(num_cells, 0);
        cell_offsets.resize(num_cells + 1, 0);
        grid_flat.resize(100000); //update to use constructor parameter later
    }

    virtual ~Solver()
    {
    	for (Thread& thread : threader.threads)
	{
		thread.stop();
	}
    
    }

   
    std::vector<Particle> objects;

    Particle& addObject(const Vec2& p_position, float radius);
    Particle& addObjectGrid(const Vec2& p_position, float radius);

    std::vector<int> grid[350][350];
    int gridsize = 5;
    
    std::vector<int> grid_flat;
    std::vector<int> cell_offsets;
    std::vector<int> cell_counts;

    void updateGridPosFlat();
    void collideCellsFlat(int x1, int y1, int x2, int y2);
    void checkCollisionsSliceFlat(int lcol, int rcol);
    void checkCollisionsGridFlat();

    void updateQuadtree();
    void updateGrid();
    void updateGridPos();

    const std::vector<Particle>& getObjects() const;

    // for a circle
    void applyBoundary();

    //this is for the borders of the window
    void applyBorder();
    void applyBorderThreaded(int index);

    void updateTree();

    std::array<float, 3> getBoundary() const;

    void setBoundary(const Vec2& position, float radius);

    void mousePull(const Vec2& position);

    void mousePush(const Vec2& position);

    void setObjectVelocity(Particle& particle, Vec2 v);

    void updateObjectsThreaded(int start, int end, float dt);
    void updateObjectsThreader(float dt);

    void checkCollisions(std::vector<std::pair<Particle*, Particle*>>& collision_pairs);
    void checkCollisionsGrid();
    void checkCollisionsSlice(int lcol, int rcol);
    


};

#endif
