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

std::string EObjectType2String(EObjectType t) {
	switch (t)	
	{
	case Wall:
		return "Wall";
	case Window:
		return "Window";
	case Door:
		return "Door";
	case PressurePlate:
		return "PressurePlate";
	default:
		return  "";
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
	
	// on regarde aussi si des objets sont aggriper sur le tile
	for (int j = 0; j < _initData.objectInfoArraySize; j++) {
		SObjectInfo o = _initData.objectInfoArray[j];
		for (auto& cell : cellDetails)
		{
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
				break;
			}
		}
	}
	for (auto& cell : cellDetails)
	{
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
	// Mettre à jour ou insérer les cellules sans détruire les nœuds existants
	// (pour garder la stabilité des pointeurs stockés dans voisins/pasVoisins)
	for (int j = 0; j < _turnData.tileInfoArraySize; j++) {
		AxialCoord coord{ _turnData.tileInfoArray[j].q, _turnData.tileInfoArray[j].r };

		// operator[] crée la cellule si elle n'existe pas, ou la récupère si elle existe déjà
		cell& c = cellDetails[coord];
		c.q = coord.q;
		c.r = coord.r;
		c.obstacle = (_turnData.tileInfoArray[j].type == Forbidden);
		c.type = _turnData.tileInfoArray[j].type;
	}

	// Pour chaque objet dans le monde, on note le voisin déconnecté
	for (int j = 0; j < _turnData.objectInfoArraySize; j++) {
		AxialCoord cellCoord{ _turnData.objectInfoArray[j].q,  _turnData.objectInfoArray[j].r };
		SObjectInfo& o = objectDetails[_turnData.objectInfoArray[j].uid];
		o = _turnData.objectInfoArray[j];


		BOT_LOGIC_LOG(mLogger, "objet : "+ std::to_string(o.uid) +" q(" + std::to_string(o.q) + ") r(" + std::to_string(o.r) + ")", true);
		for (int i = 0; i < o.connectedToSize; i++) {
			BOT_LOGIC_LOG(mLogger, std::to_string(o.connectedTo[i]), true);
		}

		auto cellIt = cellDetails.find(cellCoord);
		if (cellIt != cellDetails.end()) {
			cell& currentCell = cellIt->second;

			AxialCoord neighbourCoord = {
				currentCell.q + directions[o.cellPosition][0],
				currentCell.r + directions[o.cellPosition][1]
			};

			auto it = cellDetails.find(neighbourCoord);
			if (it != cellDetails.end()) {
				// Ajout sécurisé sans doublons pour cell -> voisin
				auto refresh = std::find(currentCell.pasVoisins.begin(), currentCell.pasVoisins.end(), &it->second);
				if (refresh == currentCell.pasVoisins.end()) {
					currentCell.pasVoisins.push_back(&it->second);
				}

				// Ajout sécurisé sans doublons pour voisin -> cell (symétrique)
				auto refreshSym = std::find(it->second.pasVoisins.begin(), it->second.pasVoisins.end(), &currentCell);
				if (refreshSym == it->second.pasVoisins.end()) {
					it->second.pasVoisins.push_back(&currentCell);
				}
			}
		}
	}

	for (auto& pair : objectDetails) {
		AxialCoord coord{ pair.second.q, pair.second.r };
		auto cellIt = cellDetails.find(coord);
		if (cellIt != cellDetails.end()) {
			cell& currentCell = cellIt->second;

			// On calcule la position du voisin bloqué par rapport à l'objet
			AxialCoord neighbourCoord = {
				currentCell.q + directions[pair.second.cellPosition][0],
				currentCell.r + directions[pair.second.cellPosition][1]
			};

			auto it = cellDetails.find(neighbourCoord);
			if (it != cellDetails.end()) {
				// Ajout sécurisé sans doublons pour cell -> voisin bloqué
				auto refresh = std::find(currentCell.pasVoisins.begin(), currentCell.pasVoisins.end(), &it->second);
				if (refresh == currentCell.pasVoisins.end()) {
					currentCell.pasVoisins.push_back(&it->second);
				}

				// Ajout sécurisé sans doublons pour voisin -> cell (symétrique)
				auto refreshSym = std::find(it->second.pasVoisins.begin(), it->second.pasVoisins.end(), &currentCell);
				if (refreshSym == it->second.pasVoisins.end()) {
					it->second.pasVoisins.push_back(&currentCell);
				}
			}
		}
	}

	// Recalculer les voisins normaux pour toutes les cellules
	for (auto& pair : cellDetails) {
		cell& currentCell = pair.second;

		for (const auto& direction : directions) {
			AxialCoord neighbourCoord = {
				currentCell.q + direction[0],
				currentCell.r + direction[1]
			};

			auto it = cellDetails.find(neighbourCoord);
			if (it != cellDetails.end()) {
				// Ajout sécurisé sans doublons (analogue à pasVoisins)
				auto found = std::find(currentCell.voisins.begin(), currentCell.voisins.end(), &it->second);
				if (found == currentCell.voisins.end()) {
					currentCell.voisins.push_back(&it->second);
				}
			}
		}
	}
}

STileInfo MyBotLogic::ClosestGoal(list<STileInfo> goals, STileInfo start) {
	unsigned int minDistance = INFINITE;
	STileInfo bestGoal = start; // Je l'assigne a Start juste parce que si il est vide le botlogic plantera
	auto startIt = cellDetails.find(AxialCoord{ start.q,start.r });

	for (STileInfo goal : goals) {
		auto it = cellDetails.find(AxialCoord{ goal.q,goal.r });
		allGoalsNpc.push_back(&it->second);	// on garde en mémoire tout les buts possible
		auto goalTest = cellDetails.find(AxialCoord{ goal.q,goal.r });
		bool hasARoute = false; //on test si on peut y accéder

		A_star(&startIt->second, &goalTest->second);
		cell* c = &goalTest->second;
		if (c->occupy) {
			continue;
		}

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

	return bestGoal;
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
		auto startIt = cellDetails.find(AxialCoord{ start.q,start.r });

		

		// de preference le but le plus proche s'il y en a un...
		STileInfo bestGoal = ClosestGoal(goals, start);

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
			bool isBlockedFromHere = std::find(currentCell->pasVoisins.begin(), currentCell->pasVoisins.end(), cellVoisin) != currentCell->pasVoisins.end();
			bool isBlockedFromThere = std::find(cellVoisin->pasVoisins.begin(), cellVoisin->pasVoisins.end(), currentCell) != cellVoisin->pasVoisins.end();

			if (isBlockedFromHere || isBlockedFromThere)
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

bool MyBotLogic::IsCulDeSac(cell* node, cell* parentNode, int currentDepth, int maxVision) {
	int validExits = 0;
	std::vector<cell*> nextCells;

	// Condition d'arrêt 1 : Si on a atteint la limite de la vision sans trouver d'impasse formelle, 
	// on donne le bénéfice du doute (on ne prune pas).
	if (currentDepth >= maxVision) {
		return false;
	}

	for (auto* v : node->voisins) {
		if (v == parentNode) continue;
		if (v->obstacle || v->occupy) continue;

		// Vérification des murs / vitres dans les deux sens
		bool blocked1 = std::find(node->pasVoisins.begin(), node->pasVoisins.end(), v) != node->pasVoisins.end();
		bool blocked2 = std::find(v->pasVoisins.begin(), v->pasVoisins.end(), node) != v->pasVoisins.end();
		if (blocked1 || blocked2) continue;

		validExits++;
		nextCells.push_back(v);
	}

	// Condition d'arrêt 2 : S'il n'y a 0 sortie, c'est une impasse sèche.
	if (validExits == 0) {
		return true;
	}

	// Condition 3 : S'il y a des choix (bifurcation), on doit tester TOUTES les branches.
	// Un chemin est un cul-de-sac seulement si *toutes* les branches qui en découlent mènent à une impasse.
	bool allBranchesAreDeadEnds = true;
	for (auto* nextCell : nextCells) {
		// Si au moins une branche n'est PAS un cul-de-sac, alors le chemin global est valide
		if (!IsCulDeSac(nextCell, node, currentDepth + 1, maxVision)) {
			allBranchesAreDeadEnds = false;
			break; // Dès qu'on trouve une issue viable, on arrête de chercher
		}
	}

	return allBranchesAreDeadEnds;
}

STileInfo MyBotLogic::CheckForGoal(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails, cell* start, int maxVision, int currentBot) {
	size_t maxCellVoisin = 0;
	cell c;
	STileInfo tile;
	bool foundTile = false;

	for (auto& v : start->voisins) {
		bool isBlockedFromHere = std::find(start->pasVoisins.begin(), start->pasVoisins.end(), v) != start->pasVoisins.end();
		bool isBlockedFromThere = std::find(v->pasVoisins.begin(), v->pasVoisins.end(), start) != v->pasVoisins.end();

		if (isBlockedFromHere || isBlockedFromThere)
			continue;

		if (!v->obstacle && !v->occupy && !v->explored) {

			if (IsCulDeSac(v, start, 1, maxVision)) {
				continue; // On ignore ce voisin car il mène à une impasse dans notre champ de vision !
			}

			if (maxCellVoisin < v->voisins.size()) {
				maxCellVoisin = v->voisins.size();
				c = *v;
				foundTile = true;
			}
		}
	}

	if (!foundTile) {
		cell* bestTarget = nullptr;
		float minScore = INFINITY;

		// On regarde si le bot a déjà vu un goal (supposons que bots[i] ou l'état stocke 'seenGoal' et la position du goal)
		bool hasSeenGoal = bots[currentBot].seenGoal;
		cell* knownGoalCell = bots[currentBot].knownGoalCell; // Pointeur vers le goal vu précédemment, si stocké

		for (auto& [pos, cellDetail] : cellDetails) {
			if (!cellDetail.explored && !cellDetail.obstacle && !cellDetail.occupy) {
				cell* cel = &cellDetail;

				// On teste si on peut y accéder depuis le start
				A_star(start, cel);

				if (cel->parent != nullptr || cel == start) {

					// Calcul du coût ou de la distance
					float score = 0.0f;

					if (hasSeenGoal && knownGoalCell != nullptr) {
						// STRATÉGIE A : Si on a vu un goal, on privilégie la cellule inexplorée 
						// qui est la PLUS PROCHE de ce goal (Distance Manhattan ou heuristique A*)
						float distToGoal = (float)(abs(cel->q - knownGoalCell->q) +
							abs(cel->r - knownGoalCell->r) +
							abs((cel->q + cel->r) - (knownGoalCell->q + knownGoalCell->r))) / 2.0f;
						score = distToGoal;
					}
					else {
						// STRATÉGIE B : Si aucun goal n'a été vu, on prend simplement la cellule inexplorée la plus proche du bot
						// (On utilise le nombre d'étapes du A* pour aller de start à cel)
						float pathCost = 0;
						cell* temp = cel;
						while (temp != start && temp->parent != nullptr) {
							pathCost += 1.0f;
							temp = temp->parent;
						}
						score = pathCost;
					}

					if (score < minScore) {
						minScore = score;
						bestTarget = cel;
					}
				}
			}
		}

		// Si on a trouvé une cible idéale selon la stratégie
		if (bestTarget != nullptr) {
			// On déroule le chemin pour trouver le premier pas à faire vers cette cible
			cell* tempCel = bestTarget;
			while (tempCel->parent != nullptr && tempCel->parent != start) {
				tempCel = tempCel->parent;
			}

			c = *tempCel; // On récupère la première étape
			foundTile = true;
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

		UpdateWorldState(_turnData);
		bots[i].UpdateState(cellDetails, startsNpc[i], goalsNpc[i]); // Je veux updater l'état de mon bot
		BOT_LOGIC_LOG(mLogger, "bot state " + BOT_STATE_2_string(bots[i].GetState()), true);

		// Selon l'état de mon bot on fais une logic différente
		if (bots[i].GetState() == BOT_STATE::GOAL_FOUND) {
			//
			for (auto c : startsNpc[i]->voisins) {
				BOT_LOGIC_LOG(mLogger, "voisin : q(" + std::to_string(c->q) + ") r(" + std::to_string(c->r) + ")", true);
			}
			for (auto c : startsNpc[i]->pasVoisins) {
				BOT_LOGIC_LOG(mLogger, "pas voisin : q(" + std::to_string(c->q) + ") r(" + std::to_string(c->r) + ")", true);
			}
			list<STileInfo> goals = {};
			for (int j = 0; j < _turnData.tileInfoArraySize; j++) {
				if (_turnData.tileInfoArray[j].type == Goal) {
					goals.push_back(_turnData.tileInfoArray[j]);
				}
			}
			STileInfo start = { startsNpc[i]->q, startsNpc[i]->r, startsNpc[i]->type };
			STileInfo bestGoal = ClosestGoal(goals, start);
			if (bestGoal.q != start.q && bestGoal.r != start.r) {
				auto goalIt = cellDetails.find(AxialCoord{ bestGoal.q,bestGoal.r });
				goalsNpc[i] = &goalIt->second;
				bots[i].knownGoalCell = &goalIt->second;
				bots[i].UpdateState(cellDetails, startsNpc[i], goalsNpc[i]); // Je veux updater l'état de mon bot
				BOT_LOGIC_LOG(mLogger, "bot state " + BOT_STATE_2_string(bots[i].GetState()), true);
			}
			else {
				bots[i].UpdateState(cellDetails, startsNpc[i], nullptr); // J'ai un but trouvé inateignable
				BOT_LOGIC_LOG(mLogger, "bot state " + BOT_STATE_2_string(bots[i].GetState()), true);
			}
			
		}
		if (bots[i].GetState() == BOT_STATE::REACHING) {
			// on regarde si on voit un goal qui est occupé que notre bot voulait aller
			// puis on lui dit qu'il peut pas et qu'il doit choisir un autre but
			if (goalsNpc[i]->occupy && startsNpc[i] != goalsNpc[i]) {
				cellDetails.at({ goalsNpc[i]->q,goalsNpc[i]->r }).obstacle = true;
				for (auto goal : allGoalsNpc) {
					if (goal != goalsNpc[i]) {
						goalsNpc[i] = goal;
						break;
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

			for (auto p : path) {
				BOT_LOGIC_LOG(mLogger,
					"Next position: q(" +
					std::to_string(p->q) + ") r(" +
					std::to_string(p->r) + ")", true);
			}

			if (path.empty() && startsNpc[i] != goalsNpc[i]) {
				BOT_LOGIC_LOG(mLogger, "Chemin introuvable (path.size() == 0), retour en SEARCHING", true);

				// On libère le goal invalide pour que la state machine comprenne qu'il n'y a plus de cible
				goalsNpc[i] = nullptr;
				bots[i].UpdateState(cellDetails, startsNpc[i], goalsNpc[i]); // Je veux updater l'état de mon bot
				BOT_LOGIC_LOG(mLogger, "bot state " + BOT_STATE_2_string(bots[i].GetState()), true);
			}
			else {
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
			

			
		}
		if (bots[i].GetState() == BOT_STATE::SEARCHING) {
			PrintWorldState();
			for (auto c : startsNpc[i]->voisins) {
				BOT_LOGIC_LOG(mLogger, "voisin : q(" + std::to_string(c->q) + ") r(" + std::to_string(c->r) + ")", true);
			}
			for (auto c : startsNpc[i]->pasVoisins) {
				BOT_LOGIC_LOG(mLogger, "pas voisin : q(" + std::to_string(c->q) + ") r(" + std::to_string(c->r) + ")", true);
			}

			//On regarde par ou on devrais aller
			STileInfo newGoal = CheckForGoal(cellDetails, startsNpc[i], _turnData.npcInfoArray[i].visionRange, i);
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