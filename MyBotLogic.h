#pragma once


// Global include
#include <vector>
#include <unordered_map>
#include <functional>



// Include of project file
#include "Globals.h"
#include "Cell.h"
#include "InitData.h"

#include "BotLogicIF.h"
#include "Logger.h"
#include "MyBotState.h"



#ifdef _DEBUG
#define BOT_LOGIC_DEBUG
#endif

#ifdef BOT_LOGIC_DEBUG
#define BOT_LOGIC_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_LOG(logger, text, autoEndLine) 0
#endif


//Custom BotLogic where the AIBot decision making algorithms should be implemented.
//This class must be instantiated in main.cpp.
class MyBotLogic : public virtual BotLogicIF
{


public:

	// Constructor and destructor of the class
	MyBotLogic();
	virtual ~MyBotLogic();


	/// <summary>
	/// Configure the log system
	/// </summary>
	/// <param name="_configData">Initial data about the level (i think) ???</param>
	void Configure(const SConfigData& _configData);


	/// <summary>
	/// Initialize the bots, their positions and their objectives
	/// </summary>
	/// <param name="_initData">Initial data</param>
	void Init(const SInitData& _initData);


	/// <summary>
	/// Decide the actions sent to the game engine
	/// </summary>
	/// <param name="_turnData">Turn data</param>
	/// <param name="_orders">List of orders to build and send to the engine</param>
	void GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders);




protected:

	// ??? Must be for debug but don't know how it works ???
	Logger mLogger;


private:

	/// <summary>
	/// Find a path between two cells
	/// </summary>
	/// <param name="start">Start cell</param>
	/// <param name="goal">Goal cell</param>
	void A_star(cell* start, cell* goal);

	/// <summary>
	/// Configure the intial representation of the map (what i understood)
	/// </summary>
	/// <param name="_initData">Level specificities and structure</param>
	void CreateWorldView(const SInitData& _initData);


	/// <summary>
	/// Update the representation of the map (what i understood)
	/// </summary>
	/// <param name="_initData">Turn specificities</param>
	void UpdateWorldView(const STurnData& _turnData);


	/// <summary>
	/// Determine the direction of the displacement between two adjacent cells
	/// </summary>
	/// <param name="from">Poinert towards the starting cell of the displacement</param>
	/// <param name="to"Poinert towards the destination cell of the displacement></param>
	/// <returns>Direction of the displacement between the two adjecent cells</returns>
	EHexCellDirection GetDirection(cell* from, cell* to);



	// Associative container : Can access elements through a key
	// (AxialCoord is the key, cell is the value and AxialCoordHash is the hash function)
	// Can access the cell thanks to its (q,r) coordinates
	std::unordered_map<AxialCoord, cell, AxialCoordHash> cellDetails;

	//////
	// Vectors of cell coordinates
	//////
	

	// Vector with all the start cordinates of the NPC (q,r)
	std::vector<cell*> startsNpc;

	// Vector of the goals associated to the NPC
	std::vector<cell*> goalsNpc;

	// Vector of all the goals available for the NPC
	std::vector<cell*> allGoalsNpc;


	//////
	// Vectors of bot states (for the state machine)
	// I think we should change the approach with a state machine by bot
	//////

	// Vector with all the states of the NPC (searching, goal_found, ...)
	std::vector<MyBotState> bots;
};