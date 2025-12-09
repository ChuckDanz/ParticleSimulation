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
	std::vector<Particle*> particles;

	float x{}; //center of the nodes
	float y{};

	float half_W{};
	float half_H{};

	std::array<std::unique_ptr<Node>, 4> children; // ORDER: top left, top right, bottom left, bottom right 


	Node(float x, float y, float hw, float hh) : particles{}, x{x}, y{y}, half_W{hw}, half_H{hh}, children{} {};
			

};

extern std::unique_ptr<Node> root;

void initialize_root();

void insert(Particle* p, Node* n);

void queryRange(Particle* p, Node* n, std::vector<Particle*>& nodes);

int getChildIndex(const Particle* p, const Node* n);

void subdivide(Node* n);

void clear(Node* n);

void clearParticles(Node* n);

Node* query(Particle* p, Node* n);

void getAllCollisionPairs(Node* n, std::vector<std::pair<Particle*, Particle*>>& pairs);

void getAllParticles(Node* n, std::vector<Particle*>& particles);

#endif
