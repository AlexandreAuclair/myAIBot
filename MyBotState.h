#pragma once

#include "cell.h"
#include "Logger.h"
#include "InitData.h"
#include "Globals.h"
#include "ConfigData.h"

#include <unordered_map>
#include <list>
#include <algorithm>

enum class BOT_STATE {
	SEARCHING,
	GOAL_FOUND,
	REACHING,
	GOAL_REACH,
	WAITING,
	DEAD
};

class MyBotState {
public:
	MyBotState();
	~MyBotState();
	BOT_STATE GetState() { return state; }
	void StartState(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails);
	void UpdateState(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails, cell *currentCell, cell *currentGoal);

private:
	BOT_STATE state;

};