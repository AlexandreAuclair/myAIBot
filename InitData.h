// INIT_DATA_H

#pragma once


// Forward declaration to use them in STurnData (we know those types exist)
struct SNPCInfo;
struct SObjectInfo;
struct STileInfo;

struct SInitData
{
	int initDelay; //in ms

	int rowCount;
	int colCount;

	int turnDelay; //in ms
	int maxTurnNb;

	bool omniscient;			// Know if the bot is omniscient in the level


	// Classic information about the structure of the level
	SNPCInfo* npcInfoArray;
	int nbNPCs;

	STileInfo* tileInfoArray;
	int tileInfoArraySize;

	SObjectInfo* objectInfoArray;
	int objectInfoArraySize;
};
