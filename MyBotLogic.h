#pragma once

#include "BotLogicIF.h"
#include "Logger.h"
#include "InitData.h"
#include "Globals.h"
#include <vector>
#include <list>
#include <unordered_map>
#include <functional>

#include "cell.h"
#include "MyBotState.h"

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

	void A_star(cell* start, cell* goal);

	void CreateWorldState(const SInitData& _initData);
	void UpdateWorldState(const STurnData& _turnData);

	EHexCellDirection GetDirection(cell* from, cell* to);
	void PrintWorldState();
	STileInfo ClosestGoal(std::list<STileInfo> goals, STileInfo start);
	STileInfo CheckForGoal(std::unordered_map<AxialCoord, cell, AxialCoordHash>& cellDetails, cell* start);


	std::unordered_map<AxialCoord, cell, AxialCoordHash> cellDetails;
	std::vector<cell*> startsNpc;
	std::vector<cell*> goalsNpc;
	std::vector<cell*> allGoalsNpc;
	std::vector<MyBotState> bots;
};