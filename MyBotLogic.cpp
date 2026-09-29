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

void MyBotLogic::Init(const SInitData& _initData)
{

	BOT_LOGIC_LOG(mLogger, "Init", true);
	const int row = _initData.rowCount;
	const int col = _initData.colCount;
	unordered_map<AxialCoord, SObjectInfo, AxialCoordHash> objectDetails;
	
	//Write Code Here
	initData = _initData;
	for (int j = 0; j < _initData.tileInfoArraySize; j++) {
		cell c;
		c.q = _initData.tileInfoArray[j].q;
		c.r = _initData.tileInfoArray[j].r;
		if (_initData.tileInfoArray[j].type == Forbidden)
			c.obstacle = true;

		cellDetails.emplace(AxialCoord{ c.q,c.r }, c);
	}

	for (int j = 0; j < _initData.tileInfoArraySize; j++) {
		SObjectInfo o = _initData.objectInfoArray[j];
		objectDetails.emplace(AxialCoord{ o.q,o.r }, o);
	}

	for (auto& cell : cellDetails)
	{
		for (const auto& direction : directions)
		{
			AxialCoord neighbourCoord2 =
			{
				cell.first.q + direction[0],
				cell.first.r + direction[1]
			};
			
			auto it = cellDetails.find(neighbourCoord2);
			auto it2 = objectDetails.find(neighbourCoord2);
			

			if (it != cellDetails.end())
			{
				cell.second.voisins.push_back(&it->second);
			}
		}
	}

	for (int x = 0; x < initData.nbNPCs; x++) {
		STileInfo start = {};
		list<STileInfo> goals = {};
		EHexCellDirection dir = {};
		for (int j = 0; j < _initData.tileInfoArraySize; j++) {
			
			if (_initData.tileInfoArray[j].type == Goal) {
				goals.push_back(_initData.tileInfoArray[j]);
			}
			if (_initData.tileInfoArray[j].q == _initData.npcInfoArray[x].q && _initData.tileInfoArray[j].r == _initData.npcInfoArray[x].r) {
				start = _initData.tileInfoArray[j];
			}
			BOT_LOGIC_LOG(mLogger, "q="+std::to_string(_initData.tileInfoArray[j].q) + ", ", false);
			BOT_LOGIC_LOG(mLogger, "r=" + std::to_string(_initData.tileInfoArray[j].r) + ", ", false);
			BOT_LOGIC_LOG(mLogger, "type="+std::to_string(_initData.tileInfoArray[j].type), true);
			
		}
		for (int j = 0; j < _initData.objectInfoArraySize; j++) {
			BOT_LOGIC_LOG(mLogger, "q=" + std::to_string(_initData.objectInfoArray[j].q) + ", ", false);
			BOT_LOGIC_LOG(mLogger, "r=" + std::to_string(_initData.objectInfoArray[j].r) + ", ", false);
			BOT_LOGIC_LOG(mLogger, "cellPosition=" + EHexCellDirection2String((EHexCellDirection)_initData.objectInfoArray[j].cellPosition), true);
		}
		
		unsigned int minDistance = INFINITE;
		STileInfo bestGoal;

		for (STileInfo goal : goals) {
			auto it = cellDetails.find(AxialCoord{ goal.q,goal.r });
			allGoalsNpc.push_back(&it->second);
			int dq = goal.q - start.q;
			int dr = goal.r - start.r;

			unsigned int distance = abs(dq) + abs(dr);

			if (distance < minDistance) {
				minDistance = distance;
				bestGoal = goal;
			}
		}
		auto startIt = cellDetails.find(AxialCoord{ start.q,start.r });
		auto goalIt = cellDetails.find(AxialCoord{ bestGoal.q,bestGoal.r });
		startsNpc.push_back(&startIt->second);
		goalsNpc.push_back(&goalIt->second);
		
	}
}

std::string EHexCellType2String(EHexCellType t) {
	switch (t)
	{
	case Goal:
		return "Goal";
	case Forbidden:
		return "Forbidden";
	case Default:
		return "Default";
	}
}


void MyBotLogic::A_star(int i) {
	for (auto& cell : cellDetails) {
		cell.second.globalGoal = INFINITY;
		cell.second.localGoal = INFINITY;
		cell.second.visited = false;
		cell.second.parent = nullptr;
	}

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

	cell* currentCell = startsNpc[i];
	startsNpc[i]->localGoal = 0.0f;
	goalsNpc[i]->globalGoal = distance(startsNpc[i], goalsNpc[i]);
	list<cell*> listNotTestedCells;
	listNotTestedCells.push_back(startsNpc[i]);

	while (!listNotTestedCells.empty() && currentCell != goalsNpc[i]) {
		listNotTestedCells.sort([](const cell* lhs, const cell* rhs) { return lhs->globalGoal < rhs->globalGoal; });

		while (!listNotTestedCells.empty() && listNotTestedCells.front()->visited)
			listNotTestedCells.pop_front();

		if (listNotTestedCells.empty())
			break;

		currentCell = listNotTestedCells.front();
		currentCell->visited = true;

		for (auto cellVoisin : currentCell->voisins)
		{
			if (cellVoisin->visited)
				continue;

			if (cellVoisin->obstacle)
				continue;

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
					distance(cellVoisin, goalsNpc[i]);

				listNotTestedCells.push_back(cellVoisin);
			}
		}

	}
}

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

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	for (int i = 0; i < initData.nbNPCs; i++) {

		if (goalsNpc[i]->occupy && startsNpc[i] != goalsNpc[i]) {
			cellDetails.at({ goalsNpc[i]->q,goalsNpc[i]->r }).obstacle = true;
			for (auto goal : allGoalsNpc) {
				if (goal != goalsNpc[i]) {
					goalsNpc[i] = goal;
				}
			}
		}

		A_star(i);
		std::vector<EHexCellDirection> pathDirections;
		std::vector<cell*> path;
		if (goalsNpc[i] != nullptr) {
			cell* c = goalsNpc[i];
			while (c->parent != nullptr) {
				EHexCellDirection dir = GetDirection(c->parent, c);
				pathDirections.push_back(dir);
				path.push_back(c);
				c = c->parent;
			}
		}

		if (pathDirections.empty())
			continue;
		cellDetails.at({ startsNpc[i]->q,startsNpc[i]->r }).occupy = false;

		if (!path.empty() && path.back()->occupy) {
			pathDirections.push_back(CENTER);
		}
		else if (!path.empty()) {
			startsNpc[i] = path.back();
			startsNpc[i]->parent = nullptr;
		}

		cellDetails.at({ startsNpc[i]->q,startsNpc[i]->r }).occupy = true;
		BOT_LOGIC_LOG(mLogger,std::to_string(initData.npcInfoArray[i].uid) + " move to " + EHexCellDirection2String(pathDirections.back()), true);
		_orders.push_back({ Move, initData.npcInfoArray[i].uid, pathDirections.back(), 0, OpenDoor});
	}
}