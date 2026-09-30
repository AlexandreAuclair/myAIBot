#pragma once

#include "BotLogicIF.h"
#include "Logger.h"
#include "InitData.h"
#include "Globals.h"
#include <vector>
#include <unordered_map>
#include <functional>

#ifdef _DEBUG
#define BOT_LOGIC_DEBUG
#endif

#ifdef BOT_LOGIC_DEBUG
#define BOT_LOGIC_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_LOG(logger, text, autoEndLine) 0
#endif

struct SConfigData;
struct STurnData;
struct SInitData;

//Custom BotLogic where the AIBot decision making algorithms should be implemented.
//This class must be instantiated in main.cpp.
class MyBotLogic : public virtual BotLogicIF
{
public:
	MyBotLogic();
	virtual ~MyBotLogic();

	virtual void Configure(const SConfigData& _configData);
	virtual void Init(const SInitData& _initData);
	virtual void GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders);

protected:
	Logger mLogger;
private:

	SInitData initData;

	struct cell {
		bool obstacle = false;
		bool visited = false;
		bool occupy = false;
		float globalGoal;
		float localGoal;
		int q;
		int r;
		std::vector<cell*> voisins;
		std::vector<cell*> pasVoisins;
		cell* parent;
	};

	void A_star(cell* start, cell* goal);


	struct AxialCoord
	{
		int q;
		int r;

		bool operator==(const AxialCoord& other) const
		{
			return q == other.q && r == other.r;
		}
	};
	struct AxialCoordHash
	{
		std::size_t operator()(const AxialCoord& coord) const
		{
			return std::hash<int>{}(coord.q) ^
				(std::hash<int>{}(coord.r) << 1);
		}
	};

	EHexCellDirection GetDirection(cell* from, cell* to);


	std::unordered_map<AxialCoord, cell, AxialCoordHash> cellDetails;
	std::vector<cell*> startsNpc;
	std::vector<cell*> goalsNpc;
	std::vector<cell*> allGoalsNpc;
};