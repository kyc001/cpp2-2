#include "../../include/core/enemycontroller.h"
#include "../../include/entities/enemy.h"
#include "../../include/core/gamestate.h"
#include "../../include/core/gamemap.h"
#include <ctime>
#include <cstdlib>

// Stage information structure
struct StageInfo
{
    int type_num;
    struct TypeInfo
    {
        int type;
        int max_nums;
        int cd_max;
        int cd_min;
        int reduce_rate;
    };
    std::vector<TypeInfo> type_info;
};

// Define stages
const StageInfo STAGE_INFOS[] = {
    {   // Stage 1
     4, // 4 types of enemies
     {
         {0, 20, 3000, 2000, 50},   // Basic melee - type, max count, max cooldown, min cooldown, reduce rate
         {1, 15, 5000, 3000, 100},  // Fast melee
         {2, 10, 10000, 5000, 200}, // Ranged
         {3, 5, 15000, 8000, 300}   // Tank
     }},
    {// Stage 2 - harder
     4,
     {{0, 30, 2500, 1500, 100},
      {1, 20, 4000, 2000, 150},
      {2, 15, 8000, 4000, 250},
      {3, 8, 12000, 6000, 350}}}};

EnemyController::EnemyController(GameState *g, int stage) : game(g), game_stage(stage)
{
    StageInfo this_stage = STAGE_INFOS[stage - 1];
    type_num = this_stage.type_num;

    // Initialize types
    int vector_pos = 0;
    for (auto &each : this_stage.type_info)
    {
        TypeController temp = {};
        temp.enemy_type = each.type;
        temp.max_nums = each.max_nums;
        temp.num_counters = 0;
        temp.max_cds = each.cd_max;
        temp.cds = each.cd_max;
        temp.min_cds = each.cd_min;
        temp.reduce_cds = each.reduce_rate;
        temp.reduce_counters = each.reduce_rate;
        temp.vector_pos = vector_pos;
        types.push_back(temp);
        vector_pos++;
    }

    enemy_death_cnt = 0;

    // Seed the random number generator
    std::srand(time(nullptr));
}

void EnemyController::tick()
{
    // Spawn enemies based on cooldowns
    for (auto &type : types)
    {
        // If this type has reached its maximum count, skip
        if (type.num_counters >= type.max_nums)
        {
            continue;
        }

        // Reduce cooldown
        type.cds -= 16; // Assuming 60fps (16ms per frame)

        // If cooldown reached, spawn a new enemy
        if (type.cds <= 0)
        {
            // Generate random position at map edge
            auto [x, y] = generatePosition();

            // Create and add the enemy
            Enemy *enemy = new Enemy(type.enemy_type, x, y, game->getMap(), game);
            game->addEnemy(enemy);

            // Update counters
            type.num_counters++;

            // Reset cooldown
            type.cds = type.max_cds;

            // Reduce cooldown for next time
            type.reduce_counters--;
            if (type.reduce_counters <= 0)
            {
                type.max_cds = std::max(type.min_cds, type.max_cds - type.reduce_cds);
                type.reduce_counters = type.reduce_cds;
            }
        }
    }
}

void EnemyController::reportDeath(int type)
{
    // Increment death counter
    enemy_death_cnt++;

    // Find the correct type controller
    for (auto &t : types)
    {
        if (t.enemy_type == type)
        {
            // Reduce count
            if (t.num_counters > 0)
            {
                t.num_counters--;
            }
            break;
        }
    }
}

void EnemyController::reportDeath(int type, double rx, double ry)
{
    Q_UNUSED(rx);
    Q_UNUSED(ry);
    // Report death and also the position for drops
    reportDeath(type);

    // Chance to spawn drops based on position
    // Drop implementation would go here
}

std::pair<double, double> EnemyController::generatePosition()
{
    // Get map dimensions from the game state
    GameMap *map = nullptr;
    if (game)
    {
        map = game->getMap(); // 假设GameState有getMap方法
    }

    int map_width = 30; // Default values if no map
    int map_height = 20;

    if (map)
    {
        map_width = map->getWidth();
        map_height = map->getHeight();
    }

    // Pick a random edge
    int edge = std::rand() % 4; // 0-3 for top, right, bottom, left
    double x, y;

    switch (edge)
    {
    case 0: // Top
        x = std::rand() % map_width;
        y = 0;
        break;
    case 1: // Right
        x = map_width - 1;
        y = std::rand() % map_height;
        break;
    case 2: // Bottom
        x = std::rand() % map_width;
        y = map_height - 1;
        break;
    case 3: // Left
        x = 0;
        y = std::rand() % map_height;
        break;
    }

    return std::make_pair(x, y);
}
