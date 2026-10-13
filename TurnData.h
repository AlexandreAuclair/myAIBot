// TURN_DATA_H

#pragma once

// Forward declaration to use them in STurnData (we know those types exist)
struct SNPCInfo;
struct SObjectInfo;
struct STileInfo;

struct STurnData
{
	int turnNb;		//N° of the turn

	// Array of NPC's and its size
	SNPCInfo* npcInfoArray;
	int npcInfoArraySize;

	// Array of cells and its size
	STileInfo* tileInfoArray;
	int tileInfoArraySize;

	// Array of objects in the level and its size
	SObjectInfo* objectInfoArray;
	int objectInfoArraySize;

	// ??? Maybe time it took to finish the turn ???
	int turnDelay; //in ms
};
