#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "Vec2.hpp"
#include <iostream>


class Particle
{
private:
    Vec2 position; //displacement vector
    Vec2 velocity;
    Vec2 acceleration;

    float mass;
    float radius;
    std::string color;

public:
    Particle();
    Particle(float pos_x, float pos_y, float mass, float radius, std::string color);
    Particle(float pos_x, float pos_y, Vec2 velocity);

    Vec2 apply_force(const Vec2& force);

    Vec2 onClick(); //this will be on hold and release of particle : calculates velocity

    void update(float dt);


};


#endif 