#ifndef ESOLVER_HPP
#define ESOLVER_HPP

#include "grid.hpp"
#include <random>
#include <cmath>

struct ESolver
{
	float BASE_THRESHOLD = 1.0f;

	int W = 600;
	int H = 600;

	static constexpr int substeps = 8;

	
	std::vector<GridCell*> active;
	std::vector<std::vector<GridCell>> grid;


	ESolver(int p_w, int p_h) : 
		W{p_w},
		H{p_h}
	{
		grid.assign(H, std::vector<GridCell>(W));
		active.reserve(H * W / 4); // reserve enough space for electric cells
	}


	void setGrid() 
	{
		// intialize the grid with default cells
		for (int r = 0; r < H; r++)
		{
			for (int c = 0; c < W; c++)
			{
				grid[r][c] = GridCell(0.0f, BASE_THRESHOLD + randNoise(0.05f), r, c); // 0.05 can be subject to change
			}
		}

		// this is pretty much useless if we do a point and click simulation
		// what we could do is make the borders really high in volatge so the gradient points
		// in other direction for point click
		// intialize the bottom border as ground
		for (int x = 0; x < W; x++)
		{
			// fixed so it doesnt clutter calculations
			grid[H - 1][x].isFixed = true;
		}

	}

	float randNoise(float amplitude = 1.0f)
	{
		static std::mt19937 rng(1337);
		static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
		return amplitude * dist(rng);	
	}
	
	// NOTE this function is statically zapping and never fades
	// change this so the electricity fades in a trail behind it
	void zap(int r, int c)
	{
		static std::random_device rd;
		static std::mt19937 rng(rd());
		static std::uniform_int_distribution<int> dist(3, 10);
		
		// select a random radius between 3 and 10 for the zap
		int radius = dist(rng);
		
		// clamp bounds 
		int rMin = std::max(0, r - radius);
		int rMax = std::min(H - 1, r + radius);
		int cMin = std::max(0, c - radius);
		int cMax = std::min(W - 1, c + radius);

		// bounds of where to draw the circle (coords)
		for (int i = rMin; i <= rMax; i++)
		{	
			for (int j = cMin; j <= cMax; j++)
			{
				//radius math
				int dr = i - r;
				int dc = j - c;

				// in the circle
				if (dr * dr + dc * dc <= radius * radius)
				{
					// high voltage zapping logic
					grid[i][j].potential = 1.0f;
					grid[i][j].isFixed = true;
					active.push_back(&grid[i][j]); // add to active grid list
				}
			}
		}
	
	}


	void update()
	{
		//physics substeps
		for (int i = 0; i < substeps; i++)
		{
			// laplace calculations here
			relaxVoltages();

			std::vector<GridCell*> candidateCells = findCandidateCells(active);
		    calcEField(candidateCells);
			probabilisticSelection(candidateCells);
			clearCandidates(candidateCells);	
		}
	
	}

	// this is naive too there is a better mathematical solution, one step at a time
	// if unstable switch to jacobi or better solution
	void relaxVoltages()
	{
		for (int r = 1; r < H - 1; r++)
		{
			for (int c = 1; c < W - 1; c++)
			{
				if (grid[r][c].isFixed == false)
				{
					float grid_N = grid[r - 1][c].potential;
					float grid_S = grid[r + 1][c].potential;
					float grid_W = grid[r][c - 1].potential;
					float grid_E = grid[r][c + 1].potential;

					grid[r][c].potential = (grid_N + grid_S + grid_W + grid_E) * 0.25f;

				}
			}
		}
	}

	// calculates the negative gradient of where the electricity best flows
	void calcEField(const std::vector<GridCell*>& candidateCells)
	{
		for (const auto& c : candidateCells)
		{
			// of the candidateCells calculate the new electric field via distance from the voltage scalar field
			int gridR = c->gridr;
			int gridC = c->gridc;

			// OOB check
			int grid_N = (gridR - 1 < 0) ? -1 : gridR - 1;
			int grid_S = (gridR + 1 > H - 1) ? -1 : gridR + 1;

			int grid_W = (gridC - 1 < 0) ? -1 : gridC - 1;
			int grid_E = (gridC + 1 > W - 1) ? -1 : gridC + 1;

			float V_N = (grid_N == -1) ? 0.0f : grid[grid_N][gridC].potential;
			float V_S = (grid_S == -1) ? 0.0f : grid[grid_S][gridC].potential;

			float V_W = (grid_W == -1) ? 0.0f : grid[gridR][grid_W].potential;
			float V_E = (grid_E == -1) ? 0.0f : grid[gridR][grid_E].potential;

			// this calculates the negative gradient(the rate of change between the neighboring voltages)
			c->EField.x = -(V_E - V_W) * 0.5f;
		    c->EField.y = -(V_S - V_N) * 0.5f;	

			c->EFieldMag = c->EField.magnitude();

		}
	
	}

	// this is using a probablistic method to determine if a cell will become conductive
	void probabilisticSelection(const std::vector<GridCell*>& candidateCells)
	{
		int eta = 2; // TEST: eta = 1 is very branchy, 2 is lightning like, 3+ is straight
		
		float total = 0.0f;

		// find the upper bound of r
		for (auto& cell : candidateCells)
		{
			total += pow(cell->EFieldMag, eta);
		}

		if (total == 0.0f)
		{
			int rand_idx = rand() % candidateCells.size();
			candidateCells[rand_idx]->conductive = true;
			candidateCells[rand_idx]->isCandidate = false;
			return;
		}
		float threshold = static_cast<float>(rand()) / RAND_MAX * total; // a randomly chose threhold between the total and 0, to make a cell conductive 
		float runningSum = 0.0f;
		
		// this will only make one cell conductive for now BUT CHANGE
		for (auto& cell : candidateCells)
		{
			runningSum += pow(cell->EFieldMag, eta);
			if (runningSum >= threshold)
			{
				cell->conductive = true;
				cell->potential = 1.0f; // set the potential to 1 for visualization purposes, this can be changed later
				cell->isFixed = true; // this cell is now fixed because it is conductive
				active.push_back(cell); // add to active list for next iteration
				break;
			}
		}
		
	}


	// return a vector of all nonconductive neighboring cells 
	std::vector<GridCell*> findCandidateCells(const std::vector<GridCell*>& active)
	{

		std::vector<GridCell*> candidateCells;
		candidateCells.reserve(active.size() * 4);


		for (const auto& ccell : active)
		{
			int gridR = ccell->gridr;
			int gridC = ccell->gridc;

			// OOB check 
			int grid_N = (gridR - 1 < 0) ? -1 : gridR - 1;
			int grid_S = (gridR + 1 > H - 1) ? -1 : gridR + 1;

			int grid_W = (gridC - 1 < 0) ? -1 : gridC - 1;
			int grid_E = (gridC + 1 > W - 1) ? -1 : gridC + 1;

			if (grid_N != -1)
			{
				if (grid[grid_N][gridC].conductive == false && grid[grid_N][gridC].isCandidate == false)
				{
					candidateCells.push_back(&grid[grid_N][gridC]);
					grid[grid_N][gridC].isCandidate = true;
				}
			}
			if (grid_S != -1)
			{
				if (grid[grid_S][gridC].conductive == false && grid[grid_S][gridC].isCandidate == false)
				{
					candidateCells.push_back(&grid[grid_S][gridC]);
					grid[grid_S][gridC].isCandidate = true;
				}
			}
			if (grid_W != -1)
			{
				if (grid[gridR][grid_W].conductive == false && grid[gridR][grid_W].isCandidate == false)
				{
					candidateCells.push_back(&grid[gridR][grid_W]);
					grid[gridR][grid_W].isCandidate = true;
				}
			}
			if (grid_E != -1)
			{
				if (grid[gridR][grid_E].conductive == false && grid[gridR][grid_E].isCandidate == false)
				{
					candidateCells.push_back(&grid[gridR][grid_E]);
					grid[gridR][grid_E].isCandidate = true;
				}
			}
			
		}

		return candidateCells;
	
	}

	void clearCandidates(std::vector<GridCell*>& candidateCells)
	{
		for (auto& cell : candidateCells)
		{
			cell->isCandidate = false;
		}
		
		candidateCells.clear();
	}


	std::vector<std::vector<GridCell>> getGrid() const
	{
		return grid;
	}










};

#endif // ESOLVER_HPP
