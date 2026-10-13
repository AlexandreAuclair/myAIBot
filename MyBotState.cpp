#include "MyBotState.h"
#include <string>


// Constructor for MyBotState
MyBotState::MyBotState() : state(BOT_STATE::SEARCHING) {

}

// Destructor for MyBotState
MyBotState::~MyBotState() {
	state = BOT_STATE::DEAD;
}


// Pas compris à quoi sert cette fonction
STileInfo MyBotState::CheckForGoal(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails, cell* start) {
	size_t maxCellVoisin = 0;
	cell c;
	STileInfo tile;

	for (auto& v : start->voisins) {
		if (maxCellVoisin < v->voisins.size() && !v->obstacle && (std::find(visiteds.begin(), visiteds.end(), v) == visiteds.end())) {
			maxCellVoisin = v->voisins.size();
			c = *v;
		}
		if (v->type == Goal) {
			c = *v;
			state = BOT_STATE::GOAL_FOUND;
			break;
		}
	}

	tile.q = c.q;
	tile.r = c.r;
	tile.type = c.type;
	visiteds.push_back(&c);

	return tile;
}

// Je vais probablement faire plus que changer le state mais pour l'instant ce sera tout

void MyBotState::GoalFound() {
	state = BOT_STATE::GOAL_FOUND;
}

void MyBotState::GoalReach() {
	state = BOT_STATE::GOAL_REACH;
}

void MyBotState::BotWaiting() {
	state = BOT_STATE::WAITING;
}

void MyBotState::GoalSearch() {
	state = BOT_STATE::SEARCHING;
}