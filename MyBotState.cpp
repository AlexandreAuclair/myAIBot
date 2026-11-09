#include "MyBotState.h"
#include <string>
#include <fstream>

MyBotState::MyBotState() : state(BOT_STATE::SEARCHING) {

}

MyBotState::~MyBotState() {
	state = BOT_STATE::DEAD;
}

/* L'etat de d�part du bot pour savoir comment il doit agir quand on va l'updater */
void MyBotState::StartState(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails) {
	for (auto cellDetail : cellDetails) {
		cell c = cellDetail.second;

		if (c.type == Goal) {
			state = BOT_STATE::REACHING;
		}
	}
	if (state != BOT_STATE::REACHING) {
		state = BOT_STATE::SEARCHING;
	}
}

void MyBotState::UpdateState(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails, cell *currentCell, cell *currentGoal) {
	if (currentGoal != nullptr && currentGoal != currentCell && !currentGoal->occupy) {
		state = BOT_STATE::REACHING;
		return;
	}

	for (auto& cellDetail : cellDetails) {
		cell c = cellDetail.second;

		if (c.type == Goal) {
			if (currentCell->q == c.q && currentCell->r == c.r) {
				state = BOT_STATE::GOAL_REACH;
			}
			if (state != BOT_STATE::GOAL_REACH && !c.occupy) {
				state = BOT_STATE::GOAL_FOUND;
				seenGoal = true;
			}
		}
	}
	if ((state != BOT_STATE::GOAL_FOUND && state != BOT_STATE::GOAL_REACH) || (state == BOT_STATE::GOAL_FOUND && currentGoal == nullptr)) {
		state = BOT_STATE::SEARCHING;
	}
	if (state == BOT_STATE::SEARCHING) {
		state = BOT_STATE::WAITING;
		for (auto v : currentCell->voisins) {
			if (!v->occupy && !v->explored && !v->obstacle) {
				state = BOT_STATE::SEARCHING;
				break;
			}
		}
		for (auto cellDetail : cellDetails) {
			cell c = cellDetail.second;
			if (!c.occupy && !c.explored && !c.obstacle) {
				state = BOT_STATE::SEARCHING;
				break;
			}
		}
	}
}