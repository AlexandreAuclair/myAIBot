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

std::string BOT_STATE_2_string(BOT_STATE s) {
	switch (s) {
	case BOT_STATE::SEARCHING:
		return "SEARCHING";
	case BOT_STATE::GOAL_FOUND:
		return "GOAL_FOUND";
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

void MyBotLogic::CreateWorldView(const SInitData& _initData) {
	for (int j = 0; j < _initData.tileInfoArraySize; j++) {
		cell c;
		c.q = _initData.tileInfoArray[j].q;
		c.r = _initData.tileInfoArray[j].r;
		if (_initData.tileInfoArray[j].type == Forbidden)
			c.obstacle = true;
		c.type = _initData.tileInfoArray[j].type;

		cellDetails.emplace(AxialCoord{ c.q,c.r }, c);
	}

	for (auto& cell : cellDetails)
	{
		for (int j = 0; j < _initData.objectInfoArraySize; j++) {
			SObjectInfo o = _initData.objectInfoArray[j];
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

void MyBotLogic::UpdateWorldView(const STurnData& _turnData) {
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

	CreateWorldView(_initData);

	for (int x = 0; x < _initData.nbNPCs; x++) {
		if(_initData.omniscient){}
		STileInfo start = {};
		MyBotState bot;
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
		STileInfo bestGoal = start;
		auto startIt = cellDetails.find(AxialCoord{ start.q,start.r });

		for (STileInfo goal : goals) {
			auto it = cellDetails.find(AxialCoord{ goal.q,goal.r });
			allGoalsNpc.push_back(&it->second);
			auto goalTest = cellDetails.find(AxialCoord{ goal.q,goal.r });
			bool hasARoute = false;
			A_star(&startIt->second, &goalTest->second);
			cell* c = &goalTest->second;
			BOT_LOGIC_LOG(mLogger, "first goal=", false);
			BOT_LOGIC_LOG(mLogger, "q=" + std::to_string(c->q) + ", ", false);
			BOT_LOGIC_LOG(mLogger, "r=" + std::to_string(c->r), true);
			while (c != nullptr) {
				BOT_LOGIC_LOG(mLogger, "cell=", false);
				BOT_LOGIC_LOG(mLogger, "q=" + std::to_string(c->q) + ", ", false);
				BOT_LOGIC_LOG(mLogger, "r=" + std::to_string(c->r), true);
				if (&startIt->second == c) {
					hasARoute = true;
				}
				c = c->parent;
			}
			int dq = goal.q - start.q;
			int dr = goal.r - start.r;

			unsigned int distance = abs(dq) + abs(dr);

			if (distance < minDistance && hasARoute) {
				minDistance = distance;
				bestGoal = goal;
				bot.GoalFound();
			}
		}
		if (goals.empty()) {
			// on a pas de goal on en invente un.
			BOT_LOGIC_LOG(mLogger, "No goal found", true);
		}
		auto goalIt = cellDetails.find(AxialCoord{ bestGoal.q,bestGoal.r });
		startsNpc.push_back(&startIt->second);
		goalsNpc.push_back(&goalIt->second);
		bots.push_back(bot);
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


void MyBotLogic::A_star(cell* start, cell* goal) {
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

	cell* currentCell = start;
	start->localGoal = 0.0f;
	goal->globalGoal = distance(start, goal);
	list<cell*> listNotTestedCells;
	listNotTestedCells.push_back(start);

	while (!listNotTestedCells.empty() && currentCell != goal) {
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
					distance(cellVoisin, goal);

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

	for (int i = 0; i < _turnData.npcInfoArraySize; i++) {
		startsNpc[i]->visited = true;
		
		if (bots[i].GetState() == BOT_STATE::GOAL_FOUND) {
			if (goalsNpc[i]->occupy && startsNpc[i] != goalsNpc[i]) {
				cellDetails.at({ goalsNpc[i]->q,goalsNpc[i]->r }).obstacle = true;
				for (auto goal : allGoalsNpc) {
					if (goal != goalsNpc[i]) {
						goalsNpc[i] = goal;
					}
				}
			}

			A_star(startsNpc[i], goalsNpc[i]);
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

			if (pathDirections.empty()) {
				bots[i].GoalReach();
				continue;
			}
			cellDetails.at({ startsNpc[i]->q,startsNpc[i]->r }).occupy = false;

			if (!path.empty() && path.back()->occupy) {
				bots[i].BotWaiting();
				//pathDirections.push_back(CENTER);
				
			}
			else if (!path.empty()) {
				startsNpc[i] = path.back();
				startsNpc[i]->parent = nullptr;
				cellDetails.at({ startsNpc[i]->q,startsNpc[i]->r }).occupy = true;
				BOT_LOGIC_LOG(mLogger, std::to_string(_turnData.npcInfoArray[i].uid) + " move to " + EHexCellDirection2String(pathDirections.back()), true);
				_orders.push_back({ Move, _turnData.npcInfoArray[i].uid, pathDirections.back(), 0, OpenDoor });
			}

			
		}
		if (bots[i].GetState() == BOT_STATE::SEARCHING) {
			// reconstruct world to find goal
			UpdateWorldView(_turnData);
			BOT_LOGIC_LOG(mLogger, "bot state " + BOT_STATE_2_string(bots[i].GetState()), true);
			STileInfo newGoal = bots[i].CheckForGoal(cellDetails, startsNpc[i]);
			BOT_LOGIC_LOG(mLogger, "cell q: " + std::to_string(newGoal.q) + " r: " + std::to_string(newGoal.r) + " type: " + std::to_string(newGoal.type), true);
			auto goalIt = cellDetails.find(AxialCoord{ newGoal.q,newGoal.r });
			if (goalIt != cellDetails.end()) {
				goalsNpc[i] = &goalIt->second;
				if (goalsNpc[i]->occupy) {
					bots[i].BotWaiting();
				}
				else { 
					goalsNpc[i]->occupy = true;
					startsNpc[i]->occupy = false;
					// order
					_orders.push_back({ Move, _turnData.npcInfoArray[i].uid, GetDirection(startsNpc[i], goalsNpc[i]), 0, OpenDoor });
					startsNpc[i] = goalsNpc[i];
				}
			}
			BOT_LOGIC_LOG(mLogger, "bot state " + BOT_STATE_2_string(bots[i].GetState()), true);
			
		}
		if (bots[i].GetState() == BOT_STATE::GOAL_REACH) {
			_orders.push_back({ Move, _turnData.npcInfoArray[i].uid, CENTER, 0, OpenDoor });
		}
		if (bots[i].GetState() == BOT_STATE::WAITING) {
			_orders.push_back({ Move, _turnData.npcInfoArray[i].uid, CENTER, 0, OpenDoor });
			for (auto& cellIt : cellDetails) {
				if (cellIt.second.type == Goal) {
					
					bots[i].GoalFound();
					break;
				}
			}
			bots[i].GoalSearch();
			
		}
		
	}
}