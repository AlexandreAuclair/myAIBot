#pragma once
#include <vector>
#include "Globals.h"



struct cell {

	// State of the cell
	bool obstacle = false;
	bool visited = false;
	bool occupy = false;


	float globalGoal;
	float localGoal;

	// Poisition on the map 
	int q;
	int r;

	// Vector of neighboring cells and not neightboring
	std::vector<cell*> voisins;
	std::vector<cell*> pasVoisins;

	// ??? Don't understand why we need the parent and even how we get it ???
	cell* parent;

	// Its type (goal, forbidden or default)
	EHexCellType type;
};



// Structure used in the unordered_map to access the details about each cell easily => O(1)
struct AxialCoord
{
	int q;
	int r;

	bool operator==(const AxialCoord& other) const
	{
		return q == other.q && r == other.r;
	}
};


// Hash function used to get the hash of the cell in the unordered_map of cells for the map
struct AxialCoordHash
{
	std::size_t operator()(const AxialCoord& coord) const
	{
		return std::hash<int>{}(coord.q) ^
			(std::hash<int>{}(coord.r) << 1);
	}
};