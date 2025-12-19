#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "particle.hpp"
#include <iostream>
#include <vector>
#include <array>

// keep dividing the quad tree if the particles in the region is greater than this
constexpr int MAX_PARTICLES = 4;

// screen size
constexpr int HEIGHT = 800;
constexpr int WIDTH = 800;


struct Node
{
	std::vector<int> particles;

	float x{}; //center of the nodes
	float y{};

	float half_W{};
	float half_H{};

	std::array<int, 4> children; // ORDER: top left, top right, bottom left, bottom right 


	Node(float x, float y, float hw, float hh) : particles{}, x{x}, y{y}, half_W{hw}, half_H{hh}, children{} { children.fill(-1);};
			

};

extern std::vector<Node> nodes;

extern int root_index;

void initialize_root();

void insert(Particle* p, int node_index, std::vector<Particle>& objects, std::vector<std::pair<Particle*, Particle*>>& collision_pairs);

void queryRange(Particle* p, int node_index, std::vector<Particle*>& node, std::vector<Particle>& objects, int min_index);

int getChildIndex(const Particle* p, const Node& n);

void subdivide(int node_i);

void clear();




#endif
