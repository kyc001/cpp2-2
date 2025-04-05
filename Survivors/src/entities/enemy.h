#pragma once
#include "../core/config.h"
#include "../core/gameData.h" // Include gameData.h for EnemyType and Entity

// #############################################################
//                      Enemy Definitions
// #############################################################
// EnemyType moved to gameData.h
// enum EnemyType
// {
//     ENEMY_TYPE_MOLTEN_MIDGET,
//     ENEMY_TYPE_BAT,
//     ENEMY_TYPE_BAT_BOSS,
//     ENEMY_TYPE_PLANT,
//     ENEMY_TYPE_PLANT_BOSS,
//     ENEMY_TYPE_MARIO_PLANT,
//     ENEMY_TYPE_HORNET,
//     ENEMY_TYPE_HORNET_BOSS,
//     ENEMY_TYPE_COUNT
// };

Entity init_enemy(EnemyType type, Vec2 pos);
void update_enemy(Entity *e, Player *p, float dt);
