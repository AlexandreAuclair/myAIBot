#include "MyBotLogic.h"

#include "ConfigData.h"
#include "InitData.h"
#include "TurnData.h"
#include "Globals.h"
#include <string>
#include <iostream>

using namespace std;

MyBotLogic::MyBotLogic()
{
	//Write Code Here
}

MyBotLogic::~MyBotLogic()
{
	//Write Code Here
}

void MyBotLogic::Configure(const SConfigData& _configData)
{
#ifdef BOT_LOGIC_DEBUG
	mLogger.Init(_configData.logpath, "MyBotLogic.log");
#endif

	BOT_LOGIC_LOG(mLogger, "Configure", true);

	//Write Code Here
}

const int directions[6][2] =
{
	{ 0, -1},
	{-1,  0},
	{-1,  1},
	{ 0,  1},
	{ 1,  0},
	{ 1, -1}
};

/* debug pour les log */
std::string EHexCellDirection2String(EHexCellDirection d) {
	switch (d)
	{
	case NE:
		return "NE";
	case NW:
		return "NW";
	case SE:
		return "SE";
	case SW:
		return "SW";
	case E:
		return "E";
	case W:
		return "W";
	case CENTER:
		return "CENTER";
	default:
		return "";
	}
}

/* debug pour les log */
std::string BOT_STATE_2_string(BOT_STATE s) {
	switch (s) {
	case BOT_STATE::SEARCHING:
		return "SEARCHING";
	case BOT_STATE::GOAL_FOUND:
		return "GOAL_FOUND";
	case BOT_STATE::REACHING:
		return "REACHING";
	case BOT_STATE::GOAL_REACH:
		return "GOAL_REACH";
	case BOT_STATE::WAITING:
		return "WAITING";
	case BOT_STATE::DEAD:
		return "DEAD";
	default:
		return "";
	}
}

/* debug pour les log */
std::string EHexCellType2String(EHexCellType t) {
	switch (t)
	{
	case Goal:
		return "Goal";
	case Forbidden:
		return "Forbidden";
	case Default:
		return "Default";
	default:
		return " ";
	}
}

/* cree un world state pour que le bot sache ou son les buts s'il y en a. */
void MyBotLogic::CreateWorldState(const SInitData& _initData) {
	// pour chaque tuile on va sauvegarder tout les types dans un unordered map
	for (int j = 0; j < _initData.tileInfoArraySize; j++) {
		cell c;
		c.q = _initData.tileInfoArray[j].q;
		c.r = _initData.tileInfoArray[j].r;
		if (_initData.tileInfoArray[j].type == Forbidden)
			c.obstacle = true;	// si jamais la tuile est rouge on note la cell comme un obstacle
		c.type = _initData.tileInfoArray[j].type;

		//on place le cell creer dans le tableau avec comme clé la valeur de q et r de la cell
		cellDetails.emplace(AxialCoord{ c.q,c.r }, c);
	}

	// on joint les voisins selon tout les directions possible
	for (auto& cell : cellDetails)
	{
		// on regarde aussi si des objets sont aggriper sur le tile
		for (int j = 0; j < _initData.objectInfoArraySize; j++) {
			SObjectInfo o = _initData.objectInfoArray[j];
			if (cell.first.q == o.q && cell.first.r == o.r) {
				// si oui alors c'est un mur sur lequel on ne peut pas passer
				AxialCoord neighbourCoord =
				{
					cell.first.q + directions[o.cellPosition][0],
					cell.first.r + directions[o.cellPosition][1]
				};
				auto it = cellDetails.find(neighbourCoord);
				if (it != cellDetails.end())
				{
					cell.second.pasVoisins.push_back(&it->second); //c'est un voisin qui est bloqué par un mur
					it->second.pasVoisins.push_back(&cell.second); //on doit lui dire que notre tuile courant est aussi bloqué par un mur
				}
			}
		}
		for (const auto& direction : directions)
		{
			AxialCoord neighbourCoord =
			{
				cell.first.q + direction[0],
				cell.first.r + direction[1]
			};

			auto it = cellDetails.find(neighbourCoord);


			if (it != cellDetails.end())
			{
				cell.second.voisins.push_back(&it->second); // maintenant on peut rajouter tous les voisins
			}
		}
	}
}

/* si le monde a changer je veux pouvoir rescanner le monde */
void MyBotLogic::UpdateWorldState(const STurnData& _turnData) {
	for (int j = 0; j < _turnData.tileInfoArraySize; j++) {
		cell c;
		c.q = _turnData.tileInfoArray[j].q;
		c.r = _turnData.tileInfoArray[j].r;
		if (_turnData.tileInfoArray[j].type == Forbidden)
			c.obstacle = true;
		c.type = _turnData.tileInfoArray[j].type;

		cellDetails.emplace(AxialCoord{ c.q,c.r }, c);
	}

	for (auto& cell : cellDetails)
	{
		cell.second.pasVoisins.clear();
		cell.second.voisins.clear();
		for (int j = 0; j < _turnData.objectInfoArraySize; j++) {
			SObjectInfo o = _turnData.objectInfoArray[j];
			if (cell.first.q == o.q && cell.first.r == o.r) {
				AxialCoord neighbourCoord =
				{
					cell.first.q + directions[o.cellPosition][0],
					cell.first.r + directions[o.cellPosition][1]
				};
				auto it = cellDetails.find(neighbourCoord);
				if (it != cellDetails.end())
				{
					cell.second.pasVoisins.push_back(&it->second);
					it->second.pasVoisins.push_back(&cell.second);
				}
			}
		}
		for (const auto& direction : directions)
		{
			AxialCoord neighbourCoord =
			{
				cell.first.q + direction[0],
				cell.first.r + direction[1]
			};

			auto it = cellDetails.find(neighbourCoord);


			if (it != cellDetails.end())
			{
				cell.second.voisins.push_back(&it->second);
			}
		}
	}
}

void MyBotLogic::Init(const SInitData& _initData)
{

	BOT_LOGIC_LOG(mLogger, "Init", true);
	const int row = _initData.rowCount;
	const int col = _initData.colCount;

	// on cree un world state pour le A*
	CreateWorldState(_initData);

	// pour chaque NPC on prend en note son point de départ et on lui regarde si on peut voir un but
	for (int x = 0; x < _initData.nbNPCs; x++) {
		if(_initData.omniscient){}
		STileInfo start = {};
		MyBotState bot;	// chaque npc a un state
		list<STileInfo> goals = {};
		EHexCellDirection dir = {};
		for (int j = 0; j < _initData.tileInfoArraySize; j++) {
			if (_initData.tileInfoArray[j].type == Goal) {
				goals.push_back(_initData.tileInfoArray[j]);
			}
			if (_initData.tileInfoArray[j].q == _initData.npcInfoArray[x].q && _initData.tileInfoArray[j].r == _initData.npcInfoArray[x].r) {
				start = _initData.tileInfoArray[j];
			}
		}
		
		unsigned int minDistance = INFINITE;
		STileInfo bestGoal = start; // Je l'assigne a Start juste parce que si il est vide le botlogic plantera
		auto startIt = cellDetails.find(AxialCoord{ start.q,start.r });

		// de preference le but le plus proche s'il y en a un...
		for (STileInfo goal : goals) {
			auto it = cellDetails.find(AxialCoord{ goal.q,goal.r });
			allGoalsNpc.push_back(&it->second);	// on garde en mémoire tout les buts possible
			auto goalTest = cellDetails.find(AxialCoord{ goal.q,goal.r });
			bool hasARoute = false; //on test si on peut y accéder

			A_star(&startIt->second, &goalTest->second);
			cell* c = &goalTest->second;

			// on déroule la route que le A* nous a fourni
			while (c != nullptr) {
				if (&startIt->second == c) {
					hasARoute = true; //on peut y accéder
				}
				c = c->parent;
			}
			// on calcul la distance
			int dq = goal.q - start.q; 
			int dr = goal.r - start.r;
			unsigned int distance = abs(dq) + abs(dr);

			if (distance < minDistance && hasARoute) {
				minDistance = distance;
				bestGoal = goal;
			}
		}

		if (goals.empty()) {
			// on a pas de goal juste me l'indiquer dans le log
			BOT_LOGIC_LOG(mLogger, "No goal found", true);
		}

		auto goalIt = cellDetails.find(AxialCoord{ bestGoal.q,bestGoal.r });
		startsNpc.push_back(&startIt->second); //on garde en mémoire tous les start
		goalsNpc.push_back(&goalIt->second); // et tous les goal

		bots.push_back(bot); // on sauve le state du npc
	}
}

/* algorithme de recherche de chemin le plus rapide */
void MyBotLogic::A_star(cell* start, cell* goal) {
	//pour chaque cell dans le worldState on prend des valeur de départ
	for (auto& cell : cellDetails) {
		cell.second.globalGoal = INFINITY;
		cell.second.localGoal = INFINITY;
		cell.second.visited = false;
		cell.second.parent = nullptr;
	}

	// petit fonction lambda pour avoir la distance manathan d'une cell
	auto distance = [](cell* a, cell* b)
	{
		int dq = a->q - b->q;
		int dr = a->r - b->r;

		return (float)(
			abs(dq) +
			abs(dr) +
			abs(dq + dr)
			) / 2.0f;
	};

	cell* currentCell = start;
	start->localGoal = 0.0f;
	goal->globalGoal = distance(start, goal);
	list<cell*> listNotTestedCells;
	listNotTestedCells.push_back(start);

	// pour chaque cell de la list qui n'est pas le but qu'on cherche on va les sorter selon leur heuristic
	while (!listNotTestedCells.empty() && currentCell != goal) {
		listNotTestedCells.sort([](const cell* lhs, const cell* rhs) { return lhs->globalGoal < rhs->globalGoal; });

		while (!listNotTestedCells.empty() && listNotTestedCells.front()->visited)
			listNotTestedCells.pop_front();

		if (listNotTestedCells.empty())
			break;

		currentCell = listNotTestedCells.front();
		currentCell->visited = true;

		// on va regarder dans les voisins
		for (auto cellVoisin : currentCell->voisins)
		{
			if (cellVoisin->visited)
				continue;

			if (cellVoisin->obstacle)
				continue;

			// on rejette les cell qui sont muré
			if (std::find(currentCell->pasVoisins.begin(), currentCell->pasVoisins.end(), cellVoisin) != currentCell->pasVoisins.end())
				continue;

			float possiblyLowerGoal =
				currentCell->localGoal +
				distance(currentCell, cellVoisin);

			if (possiblyLowerGoal < cellVoisin->localGoal)
			{
				cellVoisin->parent = currentCell;

				cellVoisin->localGoal = possiblyLowerGoal;

				cellVoisin->globalGoal =
					cellVoisin->localGoal +
					distance(cellVoisin, goal);

				listNotTestedCells.push_back(cellVoisin); // on ajoute a la list des non testé
			}
		}

	}
}

/* quand je déroule mon A* j'ai besoin de savoir de quel direction cette cell vient */
EHexCellDirection MyBotLogic::GetDirection(cell* from, cell* to)
{
	int dq = to->q - from->q;
	int dr = to->r - from->r;

	if (dq == 0 && dr == 0)
		return CENTER;

	if (dq == -1 && dr == 0)
		return NW;

	if (dq == 1 && dr == 0)
		return SE;

	if (dq == 0 && dr == -1)
		return W;

	if (dq == 0 && dr == 1)
		return E;

	if (dq == 1 && dr == -1)
		return SW;

	if (dq == -1 && dr == 1)
		return NE;

	return CENTER;
}

STileInfo CheckForGoal(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails, cell* start) {
	size_t maxCellVoisin = 0;
	cell c;
	STileInfo tile;
	bool foundTile = false;

	for (auto& v : start->voisins) {
		if (maxCellVoisin < v->voisins.size() && !v->obstacle && !v->occupy && !v->explored) {
			maxCellVoisin = v->voisins.size();
			c = *v;
			foundTile = true;
		}
	}

	if (!foundTile) {
		for (auto cellDetail : cellDetails) {
			if (!cellDetail.second.visited) {
				c = cellDetail.second;
			}
		}
	}

	tile.q = c.q;
	tile.r = c.r;
	tile.type = c.type;

	return tile;
}

void MyBotLogic::PrintWorldState() {
	for (const auto& cellDetail : cellDetails) {
		cell c = cellDetail.second;
		BOT_LOGIC_LOG(mLogger, "cell : q(" + std::to_string(c.q) + ") r(" + std::to_string(c.r) + ") type (" + EHexCellType2String(c.type) + ")", true);
	}
}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	for (int i = 0; i < _turnData.npcInfoArraySize; i++) {
		startsNpc[i]->explored = true; // Je veux qu'il rejette son départ dès qu'il commence
		//on ajoute nos nouvelle tiles au worldState
		if (bots[i].GetState() == BOT_STATE::SEARCHING) {
			UpdateWorldState(_turnData);
		}
		bots[i].UpdateState(cellDetails, startsNpc[i], goalsNpc[i]); // Je veux updater l'état de mon bot
		BOT_LOGIC_LOG(mLogger, "bot state " + BOT_STATE_2_string(bots[i].GetState()), true);

		// Selon l'état de mon bot on fais une logic différente
		if (bots[i].GetState() == BOT_STATE::GOAL_FOUND) {
			cell* goal{};
			for (auto& cellDetail : cellDetails) {
				if (cellDetail.second.type == Goal) {
					goal = &cellDetail.second;
				}
			}
			allGoalsNpc.push_back(goal);	// on garde en mémoire tout les buts possible
			auto goalTest = cellDetails.find(AxialCoord{ goal->q,goal->r });
			bool hasARoute = false; //on test si on peut y accéder

			A_star(startsNpc[i], &goalTest->second);
			cell* c = &goalTest->second;

			// on déroule la route que le A* nous a fourni
			while (c != nullptr) {
				if (startsNpc[i] == c) {
					hasARoute = true; //on peut y accéder
				}
				c = c->parent;
			}

			goalsNpc[i] = goal;
			bots[i].UpdateState(cellDetails, startsNpc[i], goalsNpc[i]); // Je veux updater l'état de mon bot
		}
		if (bots[i].GetState() == BOT_STATE::REACHING) {
			// on regarde si on voit un goal qui est occupé que notre bot voulait aller
			// puis on lui dit qu'il peut pas et qu'il doit choisir un autre but
			if (goalsNpc[i]->occupy && startsNpc[i] != goalsNpc[i]) {
				cellDetails.at({ goalsNpc[i]->q,goalsNpc[i]->r }).obstacle = true;
				for (auto goal : allGoalsNpc) {
					if (goal != goalsNpc[i]) {
						goalsNpc[i] = goal;
					}
				}
			}

			// On regarde c'est quoi le chemin a chaque tour de cette façon notre but peut être modulaire on va le trouver
			A_star(startsNpc[i], goalsNpc[i]);
			std::vector<EHexCellDirection> pathDirections;
			std::vector<cell*> path;
			//Si on la trouver
			if (goalsNpc[i] != nullptr) {
				// alors on va dérouler le chemin
				cell* c = goalsNpc[i];
				while (c->parent != nullptr) {
					EHexCellDirection dir = GetDirection(c->parent, c);
					pathDirections.push_back(dir);
					path.push_back(c);
					c = c->parent;
				}
			}

			BOT_LOGIC_LOG(mLogger,
				"Path size: " + std::to_string(path.size()), true);

			BOT_LOGIC_LOG(mLogger,
				"Current position: q(" +
				std::to_string(startsNpc[i]->q) + ") r(" +
				std::to_string(startsNpc[i]->r) + ")", true);

			if (!path.empty()) {
				BOT_LOGIC_LOG(mLogger,
					"Next position: q(" +
					std::to_string(path.back()->q) + ") r(" +
					std::to_string(path.back()->r) + ")", true);
			}

			//le cell qui est le start a un parent nullptr donc on sait que c'est notre position en ce moment
			// pathDirections nous donne le chemin qu'on doit prendre
			if (pathDirections.empty()) {
				// s'il est vide c'est qu'on est au goal
				continue;
			}
			//on libère notre ancienne position
			cellDetails.at({ startsNpc[i]->q,startsNpc[i]->r }).occupy = false;

			// si le path est pas vide et que notre prochaine case est occupé alors on attend
			if (!path.empty() && path.back()->occupy) {
				
				pathDirections.push_back(CENTER);
				
			}
			else if (!path.empty()) {
				// si le path est pas vide et que la prochaine case est pas occupé alors on modifie notre start et on occupe la case
				startsNpc[i] = path.back();
				startsNpc[i]->parent = nullptr;
				cellDetails.at({ startsNpc[i]->q,startsNpc[i]->r }).occupy = true;
				// petit log pour aider et on envoi l'ordre
				BOT_LOGIC_LOG(mLogger, std::to_string(_turnData.npcInfoArray[i].uid) + " move to " + EHexCellDirection2String(pathDirections.back()), true);
				_orders.push_back({ Move, _turnData.npcInfoArray[i].uid, pathDirections.back(), 0, OpenDoor });
			}

			
		}
		if (bots[i].GetState() == BOT_STATE::SEARCHING) {
			PrintWorldState();
			for (auto c : startsNpc[i]->voisins) {
				BOT_LOGIC_LOG(mLogger, "voisin : q(" + std::to_string(c->q) + ") r(" + std::to_string(c->r) + ")", true);
			}

			//On regarde par ou on devrais aller
			STileInfo newGoal = CheckForGoal(cellDetails, startsNpc[i]);
			auto goalIt = cellDetails.find(AxialCoord{ newGoal.q,newGoal.r });
			if (goalIt != cellDetails.end()) {
				goalsNpc[i] = &goalIt->second;
				
				goalsNpc[i]->occupy = true;
				startsNpc[i]->occupy = false;
				// order
				_orders.push_back({ Move, _turnData.npcInfoArray[i].uid, GetDirection(startsNpc[i], goalsNpc[i]), 0, OpenDoor });
				
				startsNpc[i] = goalsNpc[i];

			}
		}
		if (bots[i].GetState() == BOT_STATE::GOAL_REACH) {
			//Le bot a plus rien a faire et attend la fin du jeu
			_orders.push_back({ Move, _turnData.npcInfoArray[i].uid, CENTER, 0, OpenDoor });
		}
		if (bots[i].GetState() == BOT_STATE::WAITING) {
			//si notre bot doit attendre alors on le fait attendre
			_orders.push_back({ Move, _turnData.npcInfoArray[i].uid, CENTER, 0, OpenDoor });
			for (auto cellDetail : cellDetails) {
				BOT_LOGIC_LOG(mLogger, EHexCellType2String(cellDetail.second.type), true);
			}
		}
		
	}
}