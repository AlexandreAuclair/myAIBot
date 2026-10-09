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
	GOAL_REACH,
	WAITING,
	DEAD
};

class MyBotState {
public:
	MyBotState();
	~MyBotState();
	BOT_STATE GetState() { return state; }

	STileInfo CheckForGoal(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails, cell *start);
	void GoalFound();
	void GoalReach();
	void BotWaiting();
	void GoalSearch();
	std::vector<cell*> visiteds;

private:
	BOT_STATE state = BOT_STATE::SEARCHING;

};