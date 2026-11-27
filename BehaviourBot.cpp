#include "BehaviourBot.h"

void VerySmartGuy::RunTree()
{
}
/// @brief Vérifie si Agent est sur une case global goal
/// @return L'agent est sur une case glogal goal ou non
bool VerySmartGuy::OnGlobalGoal()
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
