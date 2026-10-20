#include "MyBotState.h"
#include <string>

MyBotState::MyBotState() : state(BOT_STATE::SEARCHING) {

}

MyBotState::~MyBotState() {
	state = BOT_STATE::DEAD;
}

/* L'etat de départ du bot pour savoir comment il doit agir quand on va l'updater */
void MyBotState::StartState(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails) {

}

void MyBotState::UpdateState(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails) {

}