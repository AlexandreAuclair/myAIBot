#pragma once
#include "Logger.h"
#include "Globals.h"

class VerySmartGuy
{
public:
    VerySmartGuy(Logger *log)
    {
        mLogger = log;
    };
    SOrder RunTree();
    bool IsOnGlobalGoal();
    bool GlobalGoalFound();
    bool KnowsPathToGlobalGoal();
    bool IsDoorClosed();
    bool IsNearDoor();
    bool IsNearPressurePlate();
    bool IsOnPressurePlate();
    bool IsPressurePlateNeeded();
    bool IsNextTileFree();
    bool IsNextToWall();
    bool IsWallAlreadyChecked();
    bool DoExplore();
    bool DoMove();
    bool DoWait();
    bool DoOpenDoor();
    bool DoOccupyTile();
    bool DoFreeTile();
    bool DoWallCheck();

protected:
    Logger *mLogger;
};