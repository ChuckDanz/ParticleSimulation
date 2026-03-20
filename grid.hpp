#ifndef GRID_HPP
#define GRID_HPP

#include "Vec2.hpp"

struct GridCell
{
	float potential;
	float breakdownThreshold; // this is no longer used
	bool conductive;
	bool isFixed; // this is for ground blocks or freshly zapped blockis
	bool isCandidate; // is this grid block eligible to be a candidate, prevents duplicate calculations
	int gridr, gridc; // r, c in the grid
	Vec2 EField; // the electric field vector 
	float EFieldMag; // magnitude of the electric field
			 

	GridCell() :
		potential{0.0f}, breakdownThreshold{1.0f},
		conductive{false}, isFixed{false}, isCandidate{false},
		gridr{0}, gridc{0}, EField{0.0f, 0.0f}, EFieldMag{0.0f} {}

	GridCell(float p_pot, float p_bt, int p_gx, int p_gy) :
		potential{p_pot},
		breakdownThreshold{p_bt},
		conductive{false},
		isFixed{false},
		isCandidate{false},
		gridr{p_gx},
		gridc{p_gy},
	    EField{0.0f, 0.0f},
	    EFieldMag{0.0f}	{}
};

#endif // GRID_HPP
