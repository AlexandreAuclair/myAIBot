#pragma once
#include <vector>
#include "Globals.h"

struct cell {
	bool obstacle = false;
	bool visited = false;
	bool occupy = false;
	float globalGoal;
	float localGoal;
	int q;
	int r;
	std::vector<cell*> voisins;
	std::vector<cell*> pasVoisins;
	cell* parent;
	EHexCellType type;
};

struct AxialCoord
{
	int q;
	int r;

	bool operator==(const AxialCoord& other) const
	{
		return q == other.q && r == other.r;
	}
};

struct AxialCoordHash
{
	std::size_t operator()(const AxialCoord& coord) const
	{
		return std::hash<int>{}(coord.q) ^
			(std::hash<int>{}(coord.r) << 1);
	}
};