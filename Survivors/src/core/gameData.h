#pragma once
#include "assets.h"
#include "config.h"
#include "../utils/colors.h"
#include "../entities/entity.h"   // 包含Entity和DamageNumber定义
#include "../entities/enemy.h"    // 包含EnemyType定义
#include "../weapons/weapon.h"   // 包含WeaponID, ActiveAttack, DamagingArea定义
#include "../entities/player.h"   // 包含Player定义
#include "../entities/pickup.h"   // 包含Pickup定义
#include "../entities/obstacle.h" // 包含Obstacle定义

// #############################################################
//                   Game State Structures
// #############################################################

enum GameStateID
{
    GAME_STATE_SELECT_HERO,
    GAME_STATE_MAIN_MENU,
    GAME_STATE_LEVEL_UP,
    GAME_STATE_RUNNING_LEVEL,
    GAME_STATE_WON,

    GAME_STATE_COUNT
};

struct WorldChunk
{
    Array<int, MAX_ENEMIES_CHUNK> enemyIndices;
};

struct GameState
{
    GameStateID state;
    bool initialized = false;
    bool quitApp = false;

    int entityIDCounter = 1;
    float totalTime;
    float spawnTimer;

    int bossSpawnIdx = 0;
    int currentSpawnBreakPoint;
    float spawnsPerSecond;
    int spawnCounter;
    int spawnRateIdx;

    bool mergeBlueCrystals;
    bool mergeGreenCrystals;

    Array<DamageNumber, MAX_DAMAGE_NUMBERS> damageNumbers;
    Array<Entity, MAX_ENEMIES> enemies;
    Array<ActiveAttack, MAX_ACTIVE_ATTACKS> activeAttacks;
    Array<DamagingArea, MAX_DAMAGING_AREAS> damagingAreas;
    Array<Pickup, MAX_PICKUPS> pickups;
    Array<Obstacle, MAX_OBSTACLES> obstacles;

    // One Chunk is 300x300, WORLD_SIZE is 1600x900
    WorldChunk worldGrid[WORLD_GRID_SIZE.x][WORLD_GRID_SIZE.y];

    Player player;
    float playerScreenEdgeDist;
};
