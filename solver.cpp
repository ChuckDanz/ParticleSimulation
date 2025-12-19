#include "solver.hpp"
#include <chrono>
#include <iostream>


Particle& Solver::addObject(const Vec2& p_position, float radius)
{
    objects.emplace_back(Particle(p_position, radius));
    objects.back().index = objects.size() - 1;
    return objects.back();
}

void Solver::update()
{
    auto frame_start = std::chrono::high_resolution_clock::now();
    
    float substep_dt = dt / substeps;
    
    double sort_time = 0, tree_time = 0, pairs_time = 0, gravity_time = 0, collision_time = 0, border_time = 0, update_time = 0;

    // SPATIAL SORT - improves cache locality
    auto t_sort_start = std::chrono::high_resolution_clock::now();
    spatialSort();
    auto t_sort_end = std::chrono::high_resolution_clock::now();
    sort_time = std::chrono::duration<double, std::milli>(t_sort_end - t_sort_start).count();

    // Build quadtree once per frame
    auto t_tree_start = std::chrono::high_resolution_clock::now();
    updateTree();
    auto t_tree_end = std::chrono::high_resolution_clock::now();
    tree_time = std::chrono::duration<double, std::milli>(t_tree_end - t_tree_start).count();

    // Build collision pairs once per frame
    auto t_pairs_start = std::chrono::high_resolution_clock::now();
    
    std::vector<Particle*> nearby_particles;
    nearby_particles.reserve(100);

    std::vector<std::pair<Particle*, Particle*>> collision_pairs;
    collision_pairs.reserve(objects.size() * 3);  // Better estimate

    int num_objects = objects.size();
    
    // Loop through all particles and find collision pairs
    for (int i = 0; i < num_objects; i++) 
    {
        Particle* p_1 = &objects[i];

        // Query nearby particles from quadtree
        nearby_particles.clear();
        queryRange(p_1, root_index, nearby_particles, objects, i + 1);

        // Add collision pairs (avoid duplicates)
        for (auto* p_2 : nearby_particles)
        { 
            if (p_1 == p_2) continue;
            if (p_2->index <= i) continue;  // Changed from < to <= for clarity
    
            collision_pairs.push_back({p_1, p_2});
        }
    }
    
    auto t_pairs_end = std::chrono::high_resolution_clock::now();
    pairs_time = std::chrono::duration<double, std::milli>(t_pairs_end - t_pairs_start).count();

    // Physics substeps
    for (int i = 0; i < substeps; i++)
    {    
        auto t1 = std::chrono::high_resolution_clock::now();
        applyGravity();
        auto t2 = std::chrono::high_resolution_clock::now();
        
        checkCollisions(collision_pairs);
        auto t3 = std::chrono::high_resolution_clock::now();
        
        applyBorder(); 
        auto t4 = std::chrono::high_resolution_clock::now();
        
        updateObjects(substep_dt);
        auto t5 = std::chrono::high_resolution_clock::now();
        
        gravity_time += std::chrono::duration<double, std::milli>(t2-t1).count();
        collision_time += std::chrono::duration<double, std::milli>(t3-t2).count();
        border_time += std::chrono::duration<double, std::milli>(t4-t3).count();
        update_time += std::chrono::duration<double, std::milli>(t5-t4).count();
    }
    
    auto frame_end = std::chrono::high_resolution_clock::now();
    double total_frame = std::chrono::duration<double, std::milli>(frame_end - frame_start).count();
    
    static int frame_count = 0;
    if (++frame_count % 60 == 0) {
        std::cout << "\n=== PERFORMANCE (" << objects.size() << " particles, " << substeps << " substeps) ===\n";
        std::cout << "  SpatialSort:     " << sort_time << " ms\n";
        std::cout << "  UpdateTree:      " << tree_time << " ms (1x per frame)\n";
        std::cout << "  BuildPairs:      " << pairs_time << " ms (" << collision_pairs.size() << " pairs found)\n";
        std::cout << "  Gravity:         " << gravity_time << " ms (" << substeps << " substeps)\n";
        std::cout << "  Collisions:      " << collision_time << " ms (" << substeps << " substeps)\n";
        std::cout << "  Border:          " << border_time << " ms (" << substeps << " substeps)\n";
        std::cout << "  UpdateObjs:      " << update_time << " ms (" << substeps << " substeps)\n";
        std::cout << "  ---\n";
        std::cout << "  Measured Total:  " << (sort_time + tree_time + pairs_time + gravity_time + collision_time + border_time + update_time) << " ms\n";
        std::cout << "  Actual Total:    " << total_frame << " ms\n";
        std::cout << "  Overhead:        " << (total_frame - (sort_time + tree_time + pairs_time + gravity_time + collision_time + border_time + update_time)) << " ms\n\n";
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

void Solver::updateTree()
{
    clear();
    initialize_root();
    for (auto& particle : objects)
    {
        insert(&particle, root_index, objects);
    }
}

// void Solver::updateTree()
// {
//     if (!root)
//     {
//         initialize_root();
//     }
//     else
//     {
//         clearParticles(root.get());  // Don't deallocate - just clear particle lists
//     }
    
//     for (auto& particle : objects)
//     {
//         insert(&particle, root.get());
//     }
// }


void Solver::spatialSort()
{
    constexpr int GRID_SIZE = 512;
    
    std::sort(objects.begin(), objects.end(), [this](const Particle& a, const Particle& b) {
        // Normalize positions to [0, 1]
        float ax = a.m_position.x / WIDTH;
        float ay = a.m_position.y / HEIGHT;
        float bx = b.m_position.x / WIDTH;
        float by = b.m_position.y / HEIGHT;
        
        // Convert to grid indices
        int gx_a = static_cast<int>(ax * GRID_SIZE);
        int gy_a = static_cast<int>(ay * GRID_SIZE);
        int gx_b = static_cast<int>(bx * GRID_SIZE);
        int gy_b = static_cast<int>(by * GRID_SIZE);
        
        // Clamp
        gx_a = std::max(0, std::min(GRID_SIZE - 1, gx_a));
        gy_a = std::max(0, std::min(GRID_SIZE - 1, gy_a));
        gx_b = std::max(0, std::min(GRID_SIZE - 1, gx_b));
        gy_b = std::max(0, std::min(GRID_SIZE - 1, gy_b));
        
        // Morton encode
        uint64_t morton_a = mortonEncode(gx_a, gy_a);
        uint64_t morton_b = mortonEncode(gx_b, gy_b);
        
        return morton_a < morton_b;
    });
    
    // Update indices after sorting
    for (size_t i = 0; i < objects.size(); ++i) {
        objects[i].index = i;
    }
}


//interleave bits of x and y to produce a Morton code (Z-order curve)
uint64_t Solver::mortonEncode(uint32_t x, uint32_t y)
{
    auto expandBits = [](uint32_t v) -> uint64_t {
        uint64_t x = v;
        x = (x | (x << 16)) & 0x0000FFFF0000FFFFull;
        x = (x | (x << 8))  & 0x00FF00FF00FF00FFull;
        x = (x | (x << 4))  & 0x0F0F0F0F0F0F0F0Full;
        x = (x | (x << 2))  & 0x3333333333333333ull;
        x = (x | (x << 1))  & 0x5555555555555555ull;
        return x;
    };
    
    return expandBits(x) | (expandBits(y) << 1);
}


const std::vector<Particle>& Solver::getObjects() const
{
    return objects;
}

//for circle boundary
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

//for circle boundary
void Solver::applyBorder()
{
    for (auto &particle : objects)
    {
        const float dampening = 0.75f;
        const Vec2 pos = particle.m_position;

        Vec2 npos = particle.m_position;
        Vec2 vel = particle.getVelocity();
        Vec2 dy = {vel.x * dampening, -vel.y};
        Vec2 dx = {-vel.x * dampening, vel.y};

        if (pos.x < particle.m_radius || pos.x + particle.m_radius > window_size) // reflect off left/right
        {
            if (pos.x < particle.m_radius) npos.x = particle.m_radius;
            if (pos.x + particle.m_radius > window_size) npos.x = window_size - particle.m_radius;
            particle.m_position = npos;
            particle.setVelocity(dx, 1.0);
        }
        if (pos.y < particle.m_radius || pos.y + particle.m_radius > window_size) //reflect off top and bottom
        {
            if (pos.y < particle.m_radius) npos.y = particle.m_radius;
            if (pos.y + particle.m_radius > window_size) npos.y = window_size - particle.m_radius;
            particle.m_position = npos;
            particle.setVelocity(dy, 1.0);
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


void Solver::setObjectVelocity(Particle& particle, Vec2 v)
{   
    particle.setVelocity(v, 1.0f);
}

void Solver::checkCollisions(std::vector<std::pair<Particle*, Particle*>>& collision_pairs)
{
    for (auto& pair : collision_pairs)
    { 
        Particle* p_1 = pair.first;
        Particle* p_2 = pair.second;

        Vec2 v = p_1->m_position - p_2->m_position;

        float distance = sqrt(v.x * v.x + v.y * v.y);
        float min_distance = p_1->m_radius + p_2->m_radius;

        if (distance < min_distance)
        {
            Vec2 n = v / distance;
            float total_mass = p_1->m_radius * p_1->m_radius + p_2->m_radius * p_2->m_radius;
            float mass_ratio = (p_1->m_radius *  p_2->m_radius) / total_mass;
            float delta = 0.5f * (min_distance - distance);

            p_1->m_position += n * (1 - mass_ratio) * delta;
            p_2->m_position -= n * mass_ratio * delta;


        }
    }
}
