#pragma once

class VerySmartGuy
{
public:
    VerySmartGuy() {

    };
    void RunTree();
    bool OnGlobalGoal();
    bool GlobalGoalFound();
    bool KnowsPathToGlobalGoal();
    bool IsDoorClosed();
    bool IsNearDoor();
    bool IsNearPressurePlate();
    bool IsOnPressurePlate();
    bool IsPressurePlateNeeded();
    bool IsNextTileFree();
    bool DoExplore();
    bool DoMove();
    bool DoWait();
    bool DoOpenDoor();
    bool DoOccupyTile();
    bool DoFreeTile();
};