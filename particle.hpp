#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "Vec2.hpp"
#include <iostream>
#include <deque>
#include <SFML/Graphics.hpp>


class Particle
{
private:
    Vec2 m_position_last;
    Vec2 m_acceleration;

    float m_mass;
   
    sf::Color m_color;

public:

    Vec2 m_position; //displacement vector
    float m_radius = 1.0f;
	
    int index = 0;

    Particle();
    Particle(const Vec2& p_position, float p_radius);

    void accelerate(const Vec2& p_acceleration);

    void setVelocity(const Vec2& p_velocity, float dt);

    void addVelocity(const Vec2& p_velocity, float dt);

    Vec2 getVelocity();

    void update(float dt);

    void setColor(sf::Color color);

    sf::Color getColor() const;


};


#endif 
