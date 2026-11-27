#include "BehaviourBot.h"

SOrder VerySmartGuy::RunTree()
{
    SOrder o;
    o.npcUID == 0; /*TODO MOD FOR NPC ID*/
    if (IsOnGlobalGoal())
    {
        mLogger->Log("This NPC is on a GlobalGoal");
        o.orderType = EOrderType::Move;
        o.direction = CENTER;
        return o;
    }
    // NextBranch
    bool doorBranch = true;
    while (doorBranch)
    {
        doorBranch = IsNearDoor();
        doorBranch = IsDoorClosed();
        o.orderType = EOrderType::Interact;
        o.direction = CENTER;
        o.objectUID = 0; /*TODO MOD FOR DOOR ID*/
        o.interactionType = EInteractionType::OpenDoor;
        return o;
    }
    // NextBranch
    bool onPressurePlateBranch = true;
    while (onPressurePlateBranch)
    {
        onPressurePlateBranch = IsOnPressurePlate();
        if (IsPressurePlateNeeded())
        {
            o.orderType = EOrderType::Move;
            o.direction = CENTER;
            return o;
        }
        if (IsNextTileFree())
        {
            // TODO GET NEXT TILE DATA FROM A*
            o.orderType = EOrderType::Move;
            o.direction = EHexCellDirection::E; /*TODO MOD FOR REAL NEXT DIRECTION*/
            DoOccupyTile();
            DoFreeTile();
            return o;
        }
        o.orderType = EOrderType::Move;
        o.direction = CENTER;
        return o;
    }
    // NextBranch
    bool pressurePlateNeededBranch = true;
    while (pressurePlateNeededBranch)
    {
        pressurePlateNeededBranch = IsPressurePlateNeeded();
        pressurePlateNeededBranch = IsNearPressurePlate();
        /*TODO GET DIRECTION OF PRESSURE PLATE*/
        o.orderType = EOrderType::Move;
        o.direction = E;
        return o;
    }
    // NextBranch
    bool goalBranch = true;
    while (goalBranch)
    {
        goalBranch = GlobalGoalFound();
        if (KnowsPathToGlobalGoal())
        {
            if (IsNextTileFree())
            {
                // TODO GET NEXT TILE DATA FROM A*
                o.orderType = EOrderType::Move;
                o.direction = EHexCellDirection::E; /*TODO MOD FOR REAL NEXT DIRECTION*/
                DoOccupyTile();
                DoFreeTile();
                return o;
            }
            o.orderType = EOrderType::Move;
            o.direction = CENTER;
            return o;
        }
        if (IsNextTileFree())
        {
            // TODO GET NEXT TILE DATA FROM A*
            o.orderType = EOrderType::Move;
            o.direction = EHexCellDirection::E; /*TODO MOD FOR REAL NEXT DIRECTION*/
            DoOccupyTile();
            DoFreeTile();
            return o;
        }
        o.orderType = EOrderType::Move;
        o.direction = CENTER;
        return o;
    }
    // NextBranch
    bool explorationBranch = true;
    while (explorationBranch)
    {
        explorationBranch = !IsMapFullyExplored();
        if (IsNextTileFree())
        {
            // TODO GET NEXT TILE DATA FROM A*
            o.orderType = EOrderType::Move;
            o.direction = EHexCellDirection::E; /*TODO MOD FOR REAL NEXT DIRECTION*/
            DoOccupyTile();
            DoFreeTile();
            return o;
        }
        o.orderType = EOrderType::Move;
        o.direction = CENTER;
        return o;
    }
    // NextBranch
    bool wallCheckBranch = true;
    while (wallCheckBranch)
    {
        if (IsNextToWall())
        {
            if (!IsWallAlreadyChecked())
            {
                DoWallCheck();
                o.orderType = EOrderType::Interact;
                o.interactionType = EInteractionType::SearchHiddenDoor;
                /*TODO MOD FOR REAL DOOR INFO*/
                o.direction = E;
                o.objectUID = 0;
                return o;
            }
        }
        if (IsNextTileFree())
        {
            // TODO GET NEXT TILE DATA FROM A*
            o.orderType = EOrderType::Move;
            o.direction = EHexCellDirection::E; /*TODO MOD FOR REAL NEXT DIRECTION*/
            DoOccupyTile();
            DoFreeTile();
            return o;
        }
        o.orderType = EOrderType::Move;
        o.direction = CENTER;
        return o;
    }
}
/// @brief Vérifie si Agent est sur une case global goal
/// @return L'agent est sur une case glogal goal ou non
bool VerySmartGuy::IsOnGlobalGoal()
{
    return false;
}
/// @brief Vérifie si Agent a trouvé un global goal
/// @return L'agent a trouver un global goal ou non
bool VerySmartGuy::GlobalGoalFound()
{
    return false;
}
/// @brief Vérifie si Agent a* peut se rendre au global goal
/// @return L'agent sait comment se rendre au global goal ou non
bool VerySmartGuy::KnowsPathToGlobalGoal()
{
    return false;
}
/// @brief Vérifie si Agent détecte une porte fermée. DOIT ÊTRE EFFECTUER APRES ISNEARDOOR()
/// @return Si la porte déterte est fermée
bool VerySmartGuy::IsDoorClosed()
{
    return false;
}
/// @brief Vérifie si Agent est près d’une porte manuelle
/// @return Retourne si une porte manuelle est proche ou non
bool VerySmartGuy::IsNearDoor()
{
    return false;
}
/// @brief Vérifie si Agent est près d’une plaque de pression
/// @return Retourne si une plaque de pression est proche ou non
bool VerySmartGuy::IsNearPressurePlate()
{
    return false;
}
/// @brief Vérifie si la position actuelle du NPC est sur une place de pression
/// @return Retourne si le NPC se trouve sur une plaque de pression ou non
bool VerySmartGuy::IsOnPressurePlate()
{
    return false;
}
/// @brief Vérifie si un NPC a besoin d'une activation de plaque de pression
/// @return Retourne si il y a eu une demande d'activation pour une plaque de pression ou non
bool VerySmartGuy::IsPressurePlateNeeded()
{
    return false;
}
/// @brief Vérifie si la prochaine tuile est occupé
/// @return Retourne si la prochaine tuile est occupé ou non
bool VerySmartGuy::IsNextTileFree()
{
    return false;
}
/// @brief Vérifie si l'agent est près d'un mur
/// @return Retourne si l'agent touche a un mur ou non
bool VerySmartGuy::IsNextToWall()
{
    return false;
}
/// @brief Vérifie si le mur près de l'agent a déjà été vérifier pour une porte caché
/// @return Retourne si le mur doit être vérifier ou non
bool VerySmartGuy::IsWallAlreadyChecked()
{
    return false;
}
/// @brief Envoie le NPC sur une tuile connue mais pas explorer selon l'euristique
/// @return Retourne si le NPC a réussi a explorer ou non
bool VerySmartGuy::DoExplore()
{
    return false;
}
/// @brief Envoie le NPC sur une tuile précise connue
/// @return Retourne si le NPC a réussi a se déplacer sur la tuile ou non
bool VerySmartGuy::DoMove()
{
    return false;
}
/// @brief Demande au NPC de rester sur place pendant ce tour
/// @return Retourne si le NPC a réussi a ne pas se déplacer ou non
bool VerySmartGuy::DoWait()
{
    return false;
}
/// @brief Demande au NPC d'ouvrir la porte a proximité
/// @return Retourne si l'ouverture de la porte a réussi ou non
bool VerySmartGuy::DoOpenDoor()
{
    return false;
}
/// @brief Deamnde au NPC d'occuper une tuile dans le world state après son déplacement
/// @return Est-ce que l'occupation dans le world state a réussi ou non
bool VerySmartGuy::DoOccupyTile()
{
    return false;
}
/// @brief Deamnde au NPC de libérer la tuire dans le world state apres son déplacement
/// @return Est-ce que le NPC a réussi a libérer sa tuile dans le world state ou non
bool VerySmartGuy::DoFreeTile()
{
    return false;
}
/// @brief Demande au NPC de vérifier si le mur proche de lui est une porte
/// @return Est-ce que le NPC a réussi a vérifier le mur
bool VerySmartGuy::DoWallCheck()
{
    return false;
}
