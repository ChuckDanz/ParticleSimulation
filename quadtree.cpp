#include "quadtree.hpp"

std::unique_ptr<Node> root = nullptr;

constexpr float EPS = 1e-6f;


void initialize_root()
{
	root = std::make_unique<Node>(WIDTH/2, HEIGHT/2, WIDTH/2, HEIGHT/2);
}

void insert(Particle* p, Node* n)
{

    // Safety: if particle is outside this node, don't insert
    if (p->m_position.x < n->x - n->half_W || p->m_position.x > n->x + n->half_W ||
        p->m_position.y < n->y - n->half_H || p->m_position.y > n->y + n->half_H)
    {
        return;  // particle outside bounds, skip
    }

	// if the node has children, place particles in correct children
	if (n->children[0] != nullptr)
	{
		
        int index = getChildIndex(p, n);
        insert(p, n->children[index].get());
        return;
    }
  
    //place if no children
    n->particles.push_back(p);

    if (n->particles.size() > MAX_PARTICLES && n->half_W > 4.0f && n->half_H > 4.0f)
    {
        subdivide(n);
        std::vector<Particle*> copy = n->particles;
        n->particles.clear();

        for (auto& childParticle : copy)
        {		
            int index = getChildIndex(childParticle, n);
            insert(childParticle, n->children[index].get());
        }
    }
	

}

int getChildIndex(const Particle* p, const Node* n)
{
	
	int index = 0;

	if (p->m_position.x >= n->x - EPS) index += 1;
	if (p->m_position.y >= n->y - EPS) index += 2;

	return index;
}


void subdivide(Node* n)
{
	float hw = n->half_W / 2.0f;
    float hh = n ->half_H / 2.0f;
    // top left
    n->children[0] = std::make_unique<Node>(n->x - hw, n->y - hh, hw, hh); // - -

    // top right
    n->children[1] = std::make_unique<Node>(n->x + hw, n->y - hh, hw, hh); // + -

    // bottom left
    n->children[2] = std::make_unique<Node>(n->x - hw, n->y + hh, hw, hh); // - + 

    // bottom right
    n->children[3] = std::make_unique<Node>(n->x + hw, n->y + hh, hw, hh); // + +

	
}

void clearParticles(Node* n)
{
    if (!n) return;
    n->particles.clear();
    for (auto& child : n->children)
    {
        if (child) clearParticles(child.get());
    }
}



void clear(Node* n)
{

	if (!n) return;


	for (auto& child : n->children)
	{	
		if (child)
		{
			clear(child.get());
			child.reset();
		}
	}
	n->particles.clear();		
}

Node* query(Particle* p, Node* n)
{
	 if (!n) return nullptr;

    if (p->m_position.x < n->x - n->half_W || p->m_position.x > n->x + n->half_W ||
        p->m_position.y < n->y - n->half_H || p->m_position.y > n->y + n->half_H)
    {
        return nullptr;
    }

    if (n->children[0] != nullptr)
    {
        int idx = getChildIndex(p, n);
        return query(p, n->children[idx].get());
    }

    return n;
}

void queryRange(Particle* p, Node* n, std::vector<Particle*>& nodes)
{
    if (!n) return;

    float px = p->m_position.x;
    float py = p->m_position.y;
    float pr = 2 * p->m_radius;

    // Node bounds
    float left   = n->x - n->half_W;
    float right  = n->x + n->half_W;
    float top    = n->y - n->half_H;
    float bottom = n->y + n->half_H;

    // AABB overlap test: skip if particle circle doesn't overlap this node
    if (px + pr < left  || px - pr > right ||
        py + pr < top   || py - pr > bottom)
    {
        return;  // no overlap, prune this branch
    }

    


    if (n->children[0] == nullptr)
    {
        for (auto& particle : n->particles)
        {
            nodes.push_back(particle);
        }
        //std::cout << nodes.size() << " Particles queried\n";
    }
    else
    {
        for (auto& child : n->children)
        {
            if (child)
            {
                  queryRange(p, child.get(), nodes);
            }
          
        }
    }
}

void getAllCollisionPairs(Node* n, std::vector<std::pair<Particle*, Particle*>>& pairs)
{
	if (!n) return;
	
	if (n->children[0] == nullptr)
	{
		for (int i = 0; i < n->particles.size(); i++)
		{
			for (int j = i + 1; j < n->particles.size(); j++)
			{
				pairs.push_back({n->particles[i], n->particles[j]});
			}
		}
	
	}
	else
	{
	
		for (auto& child : n->children)
		{
			if (child)
			{
				getAllCollisionPairs(child.get(), pairs);
			}
		}
		

		for (int i = 0; i < 4; i++)
		{
			if (!n->children[i]) continue;
			
			for (int j = i + 1; j < 4; j++)
			{
				if (!n->children[j]) continue;

				std::vector<Particle*> particles_i;
				std::vector<Particle*> particles_j;

				getAllParticles(n->children[i].get(), particles_i);
				getAllParticles(n->children[j].get(), particles_j);

                for (auto* p1 : particles_i)
                {
                    for (auto* p2 : particles_j)
                    {
                        Vec2 v = p1->m_position - p2->m_position;
                        float max_dist = (p1->m_radius + p2->m_radius) * 2.0f;
                        if (v.x * v.x + v.y * v.y < max_dist * max_dist)
                        {
                            pairs.push_back({p1, p2});
                        }
                    }
                }
			
			
			}
		
		}
	
	}





}



void getAllParticles(Node* n, std::vector<Particle*>& particles)
{
	if (!n) return;

	if (n->children[0] == nullptr)
	{
		particles.insert(particles.end(), n->particles.begin(), n->particles.end());
	}
	else
	{
		
			for (auto& child : n->children)
			{
                if (child)
		        {
				    getAllParticles(child.get(), particles);
    			}
		
		}
	}

}














