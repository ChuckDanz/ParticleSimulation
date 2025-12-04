#include "quadtree.hpp"

std::unique_ptr<Node> root = nullptr;


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

	if (p->m_position.x >= n->x) index += 1;
	if (p->m_position.y >= n->y) index += 2;

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
    float pr = p->m_radius;

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
















