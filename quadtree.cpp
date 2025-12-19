#include "quadtree.hpp"

std::vector<Node> nodes;
int root_index = -1;

constexpr float EPS = 1e-6f;


void initialize_root()
{
    nodes.clear();
    nodes.reserve(100000);
	nodes.emplace_back(Node{WIDTH/2, HEIGHT/2, WIDTH/2, HEIGHT/2});
    root_index = 0;
}

void insert(Particle* p, int node_index, std::vector<Particle>& objects, std::vector<std::pair<Particle*, Particle*>>& collision_pairs)
{

    if (node_index < 0 || node_index >= nodes.size()) return;  // Safety

    Node& n = nodes.at(node_index);

    // Safety: if particle is outside this node, don't insert
    if (p->m_position.x < n.x - n.half_W || p->m_position.x > n.x + n.half_W ||
        p->m_position.y < n.y - n.half_H || p->m_position.y > n.y + n.half_H)
    {
        return;  // particle outside bounds, skip
    }

	// recursion step to go into children
	if (n.children[0] == -1)
	{
		for (int q_index : n.particles)
        {
            Particle* q = &objects[q_index];

            // Only generate pair (q, p) if q.index < p.index
            if (q->index < p->index)
                collision_pairs.emplace_back(q, p);
        }   
    }
    

    //place if no children
    n.particles.push_back(p->index);

    if (n.particles.size() > MAX_PARTICLES && n.half_W > 4.0f && n.half_H > 4.0f)
    {
        subdivide(node_index); 

        // Re-insert particles
        auto old = std::move(n.particles);
        for (int idx : old)
            insert(&objects[idx], n.children[getChildIndex(&objects[idx], n)], objects, collision_pairs);
       
    }
	
    return;

}

int getChildIndex(const Particle* p, const Node& n)
{
	
	int index = 0;

	if (p->m_position.x >= n.x - EPS) index += 1;
	if (p->m_position.y >= n.y - EPS) index += 2;

	return index;
}


void subdivide(int node_i)
{   
    // Copy values BEFORE emplace_back
    float x = nodes[node_i].x;
    float y = nodes[node_i].y;
    float hw = nodes[node_i].half_W / 2.0f;
    float hh = nodes[node_i].half_H / 2.0f;

    // DON'T get reference to 'n' until after ALL emplace_backs!
    
    // top left
    nodes.emplace_back(Node{x - hw, y - hh, hw, hh});
    nodes[node_i].children[0] = nodes.size() - 1;  // Get FRESH reference each time

    // top right
    nodes.emplace_back(Node{x + hw, y - hh, hw, hh});
    nodes[node_i].children[1] = nodes.size() - 1;

    // bottom left
    nodes.emplace_back(Node{x - hw, y + hh, hw, hh});
    nodes[node_i].children[2] = nodes.size() - 1;
     
    // bottom right
    nodes.emplace_back(Node{x + hw, y + hh, hw, hh});
    nodes[node_i].children[3] = nodes.size() - 1;
}


void clear()
{
    nodes.clear();
    nodes.reserve(1000);	
    root_index = -1;	
}


void queryRange(Particle* p, int node_index, std::vector<Particle*>& node, std::vector<Particle>& objects, int min_index)
{

    if (node_index < 0 || node_index >= nodes.size()) return;

    Node& n = nodes.at(node_index);

    float px = p->m_position.x;
    float py = p->m_position.y;
    float pr = 2 * p->m_radius;

    // Node bounds
    float left   = n.x - n.half_W;
    float right  = n.x + n.half_W;
    float top    = n.y - n.half_H;
    float bottom = n.y + n.half_H;

    // AABB overlap test: skip if particle circle doesn't overlap this node
    if (px + pr < left  || px - pr > right ||
        py + pr < top   || py - pr > bottom)
    {
        return;  // no overlap, prune this branch
    }


    if (n.children[0] == -1)
    {
        for (int index : n.particles)
        {
            if (index >= min_index)
                node.push_back(&objects.at(index));
        }
        //std::cout << nodes.size() << " Particles queried\n";
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
         	int child = n.children[i];

            if (child != -1)   // skip empty children
            {
                queryRange(p, child, node, objects, min_index);
            }
        }
    }
}















