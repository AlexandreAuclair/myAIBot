// GLOBALS_H

#pragma once


// Possible cell type
enum EHexCellType
{
	Goal,
	Forbidden,
	Default
};

// Possible movement directions from a cell
enum EHexCellDirection
{
	W, NW, NE, E, SE, SW, CENTER
};

// Possible object type
enum EObjectType
{
	Wall,
	Window,
	Door,
	PressurePlate
};


// Possible object state (for instance : a door)
enum EObjectState
{
	Opened,
	Closed
};


// Possible order type for the NPC (for instance : door closed => interact with the door)
enum EOrderType
{
	Move,
	Interact
};


// ???? Don't know what is its purpose yet ???
enum EInteractionType
{
	OpenDoor,
	CloseDoor,
	SearchHiddenDoor,
};


// Order structure
struct SOrder
{
	EOrderType orderType;
	int npcUID;							// NPC's ID to whom the order is given
	EHexCellDirection direction;
	int objectUID;						// Object's ID to interact with if needed
	EInteractionType interactionType;
};


// Tile info
struct STileInfo
{
	int q;
	int r;
	EHexCellType type;
};

// Object info
struct SObjectInfo
{
	int uid;			// Unique uid to identify the object
	int q;
	int r;

	// Maybe position from the bot cell
	EHexCellDirection cellPosition;

	// ??? Don't know their purposes yet ???
	int* types;
	int typesSize;

	int* states;
	int statesSize;

	int* connectedTo;
	int connectedToSize;
};


// NPC info 
struct SNPCInfo
{
	int uid;	// unique uid to identify the bot
	int q;		// position value 1
	int r;		// position value 2
	int visionRange;		// vision because sometime the bot is not omniscient
};
