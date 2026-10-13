#pragma once

#include "Cell.h"
#include "Logger.h"
#include "InitData.h"
#include "Globals.h"
#include "ConfigData.h"

#include <unordered_map>
#include <list>
#include <algorithm>


// Enum class with possible states of the bot
enum class BOT_STATE {
	SEARCHING,
	GOAL_FOUND,
	GOAL_REACH,
	WAITING,
	DEAD
};

class MyBotState {

public:

	MyBotState();
	~MyBotState();


	// GETTER state of the bot
	BOT_STATE GetState() const { return state; }


	// ??? Pas compris à quoi sert la fonction ??? //
	STileInfo CheckForGoal(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails, cell *start);
	
	// Function updating the state of the bot depending on the situation (will be modified later i think !!!)
	void GoalFound();
	void GoalReach();
	void BotWaiting();
	void GoalSearch();

	// Vector of visited cells (used for A* algorith i think)
	std::vector<cell*> visiteds;


	
	// Members of the class 
private:
	BOT_STATE state = BOT_STATE::SEARCHING; // default state = searching for the bot

};