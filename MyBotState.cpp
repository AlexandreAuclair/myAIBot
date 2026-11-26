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

void MyBotState::UpdateState(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails, std::unordered_map<int, SObjectInfo>& objectDetails, cell *currentCell, cell *currentGoal) {
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
		bool hasTarget = false;

		// 1. Vérifier les voisins directs
		for (auto v : currentCell->voisins) {
			if (!v->occupy && !v->explored && !v->obstacle) {
				hasTarget = true;
				break;
			}
		}

		// 2. Sinon, vérifier toute la carte connue (passage par référence const pour éviter la copie)
		if (!hasTarget) {
			for (const auto& [coord, c] : cellDetails) {
				if (!c.occupy && !c.explored && !c.obstacle) {
					hasTarget = true;
					break;
				}
			}
		}

		// 3. Sinon, vérifier s'il reste une porte fermée à ouvrir
		if (!hasTarget) {
			for (const auto& [id, obj] : objectDetails) {
				for (int x = 0; x < obj.typesSize; x++) {
					if (static_cast<EObjectType>(obj.types[x]) == Door) {
						// Optionnel : vérifier ici que la porte n'est pas déjà ouverte
						hasTarget = true;
						break;
					}
				}
				if (hasTarget) break;
			}
		}

		state = hasTarget ? BOT_STATE::SEARCHING : BOT_STATE::WAITING;
	}
}