#include "assets.h"
#include "common.h"
#include "../utils/colors.h"
#include "../utils/easing_functions.h"
#include "gameData.h"
#include "input.h"
#include "shared.h"
#include "../ui/texts.h"
#include "../utils/my_math.h"
#include "../utils/sound.h"
#include "game.h"
#include "../entities/entity.h"
#include "../entities/player.h"
#include "../weapons/weapon.h"
#include "../entities/enemy.h"
#include "../entities/pickup.h"
#include "../entities/obstacle.h"

// Needed for Fucking rand() function, CRINGE
#include <cstdLib>

// #############################################################
//                   Internal Functions
// #############################################################
GLOBAL GameState *gameState = 0;
#include "../rendering/render_interface.h"
// RenderData全局变量已在render_interface.cpp中定义
#include "../rendering/render_interface.cpp"

internal Vec2 get_screen_pos(Vec2 pos)
{
    Vec2 screenMiddle = vec_2(SCREEN_SIZE) / 2.0f;
    Vec2 screenPos = screenMiddle + pos - gameState->player.pos;

    return screenPos;
}

internal void spawn_enemy(EnemyType type, bool boss = false)
{
    Entity e = {};

    float randomAngle = (float)(rand() % 360) * 3.14f / 180.0f;

    Vec2 playerPos = gameState->player.pos;
    Vec2 spawnDirection = {cosf(randomAngle), sinf(randomAngle)};
    Vec2 spawnPos = playerPos + spawnDirection * gameState->playerScreenEdgeDist;

    e.boss = boss;
    e.pos = spawnPos;
    e.ID = gameState->entityIDCounter++;

    switch (type)
    {
    case ENEMY_TYPE_MOLTEN_MIDGET:
    {
        e.spriteID = SPRITE_ENEMY_01;
        e.hp = 5;
        e.attack = 10;
        break;
    }
    case ENEMY_TYPE_BAT:
    {
        e.spriteID = SPRITE_ENEMY_BAT;
        e.hp = 8;
        e.attack = 10;
        e.moveSpeed = 150.0f;
        break;
    }
    case ENEMY_TYPE_BAT_BOSS:
    {
        e.spriteID = SPRITE_ENEMY_BAT_BOSS;
        e.hp = 80;
        e.attack = 20;
        e.moveSpeed = 100.0f;
        break;
    }
    case ENEMY_TYPE_PLANT:
    {
        e.spriteID = SPRITE_ENEMY_PLANT;
        e.hp = 10;
        e.attack = 10;
        e.moveSpeed = 50.0f;
        break;
    }
    case ENEMY_TYPE_PLANT_BOSS:
    {
        e.spriteID = SPRITE_ENEMY_PLANT_BOSS;
        e.hp = 200;
        e.attack = 10;
        e.moveSpeed = 50.0f;
        break;
    }
    case ENEMY_TYPE_MARIO_PLANT:
    {
        e.spriteID = SPRITE_ENEMY_MARIO_PLANT;
        e.hp = 40;
        e.attack = 10;
        e.moveSpeed = 25.0f;
        break;
    }
    case ENEMY_TYPE_HORNET:
    {
        e.spriteID = SPRITE_ENEMY_HORNET;
        e.hp = 10;
        e.attack = 10;
        e.moveSpeed = 200.0f;
        break;
    }
    case ENEMY_TYPE_HORNET_BOSS:
    {
        e.spriteID = SPRITE_ENEMY_HORNET_BOSS;
        e.hp = 400;
        e.attack = 10;
        e.moveSpeed = 200.0f;
        break;
    }
    }

    gameState->enemies.add(e);
}

// 公共接口函数
void spawn_enemy(EnemyType type, Vec2 pos)
{
    spawn_enemy(type, false);
}

internal void player_add_weapon(WeaponID ID, int level = 1)
{
    Weapon w = {};
    w.ID = ID;
    w.level = level;

    // TODO: We can't add inifite atcive weapons, some HAVE to be passive effects
    gameState->player.weapons.add(w);
}

// 公共接口函数
void player_add_weapon(WeaponID ID)
{
    player_add_weapon(ID, 1);
}

internal Circle get_pickup_collider(Player p)
{
    return {p.pos + p.collider.pos, p.pickupRadius};
}

internal Circle get_pickup_trigger_collider(Player p)
{
    return {p.pos + p.collider.pos, p.pickupTriggerRadius};
}

internal Circle get_collider(Player p)
{
    return {p.pos + p.collider.pos, p.collider.radius};
}

internal Circle get_collider(Entity e)
{
    return {e.pos + e.collider.pos, e.collider.radius};
}

// 公共接口函数
Circle get_collider(Player *p)
{
    return get_collider(*p);
}

// 公共接口函数
Circle get_collider(Entity *e)
{
    return get_collider(*e);
}

internal Vec2 get_chunk_offset(Vec2 pos)
{
    Vec2 tileSize = Vec2{64.0f, 64.0f} * UNIT_SCALE;
    float chunkWidth = (float)MAP_CHUNK_TILE_COUNT * tileSize.x;
    float chunkOffsetX = fmodf(pos.x, chunkWidth);
    float chunkOffsetY = fmodf(pos.y, chunkWidth);
    
    // Loop inside the Chunk
    {
        if(chunkOffsetX < 0.0f)
        {
            chunkOffsetX += chunkWidth;
        }
        
        if(chunkOffsetY < 0.0f)
        {
            chunkOffsetY += chunkWidth;
        }
    }
    
    return {chunkOffsetX, chunkOffsetY};
}

// 仅保留内部函数，删除公共接口函数的实现（避免重复定义）
internal Weapon* internal_get_weapon(WeaponID weaponID)
{
    Weapon *w = 0;

    for (int weaponIdx = 0; weaponIdx < gameState->player.weapons.count; weaponIdx++)
    {
        Weapon *weapon = &gameState->player.weapons[weaponIdx];

        if (weapon->ID == weaponID)
        {
            w = weapon;
            break;
        }
    }

    return w;
}

// 外部公共接口通过调用内部函数实现
Weapon *get_weapon(WeaponID weaponID)
{
    return internal_get_weapon(weaponID);
}

internal bool has_hit_enemy(DamagingArea da, int enemyID)
{
    bool hit = false;
    for (int hitIdx = 0; hitIdx < da.hitEnemyIDs.count; hitIdx++)
    {
        if (da.hitEnemyIDs[hitIdx] == enemyID)
        {
            hit = true;
            break;
        }
    }

    return hit;
}

// 公共接口函数
bool is_damaging_area_hitting_entity(DamagingArea *da, Entity *e)
{
    return has_hit_enemy(*da, e->ID);
}

// 仅保留内部函数，删除公共接口函数的实现（避免重复定义）
internal void internal_inflict_damage(Entity* e, int dmg)
{
    e->hp -= dmg;

    DamageNumber dn = {};
    dn.pos = e->pos;
    dn.value = dmg;
    gameState->damageNumbers.add(dn);

    if (e->hp <= 0)
    {
        // Add xp crystal
        Pickup p = {};
        p.pos = e->pos;
        
        if (e->boss)
        {
            p.type = PICKUP_TYPE_EXP_RED;
        }
        else
        {
            p.type = PICKUP_TYPE_EXP_BLUE;
        }

        gameState->pickups.add(p);
    }
}

// 外部公共接口通过调用内部函数实现
void inflict_damage(Entity *e, int damage)
{
    internal_inflict_damage(e, damage);
}

internal void internal_add_damaging_area(WeaponID weaponID, SpriteID spriteID, Vec2 pos,
                            Vec2 size, int damage, float duration)
{
    DamagingArea da = {};

    da.pos = pos;
    da.size = size;
    da.damage = damage;
    da.duration = duration;
    da.weaponID = weaponID;
    da.spriteID = spriteID;

    gameState->damagingAreas.add(da);
}

// 公共接口函数
void add_damaging_area(WeaponID weaponID, Vec2 pos, Vec2 size, int damage, float duration)
{
    internal_add_damaging_area(weaponID, SPRITE_WHITE, pos, size, damage, duration);
}

internal void draw_exp_bar()
{
    // 经验条
    {
        draw_sprite(SPRITE_EXP_BAR_LEFT, {4.0f, 16.0f}, {8.0f, 32.0f});
        draw_sprite(SPRITE_EXP_BAR_MIDDLE, {input->screenSize.x / 2.0f, 16.0f},
                    {input->screenSize.x - 16.0f, 32.0f});
        draw_sprite(SPRITE_EXP_BAR_RIGHT, {input->screenSize.x - 4.0f, 16.0f}, {8.0f, 32.0f});
        
        int level = min(gameState->player.level, ARRAY_SIZE(expTable) - 1);
        float expNeeded = (float)expTable[level];
        float barSizeX = (input->screenSize.x - 15.0f) * (float)gameState->player.exp / expNeeded;
        draw_sprite(SPRITE_WHITE, {barSizeX / 2.0f + 6.0f, 15.0f}, {barSizeX, 18.0f}, {.color = COLOR_BLUE});
    }
}

internal void update_level(float dt);

__declspec(dllexport) void init_game(GameState* gameStateIn, Input* inputIn,
                                     RenderData* renderDataIn)
{
    gameState = gameStateIn;
    input = inputIn;
    renderData = renderDataIn;
    
    if (!gameState->initialized)
    {
        gameState->initialized = true;
    }
}

__declspec(dllexport) void update_game(GameState* gameStateIn, Input* inputIn,
                                       RenderData* renderDataIn, float dt)
{
    gameState = gameStateIn;
    input = inputIn;
    renderData = renderDataIn;
    
    // 添加游戏更新逻辑...
    
    // 游戏状态机处理
    switch (gameState->state)
    {
    case GAME_STATE_RUNNING_LEVEL:
        update_level(dt);
        break;
    
    // 添加其他游戏状态处理...
    }
}

internal void update_level(float dt)
{
    // 实现level更新逻辑...
    
    // Boss生成逻辑
    struct BossSpawn
    {
        float time;
        EnemyType enemyType;
    };
    
    static BossSpawn bossSpawns[] = {
        {60.0f, ENEMY_TYPE_BAT_BOSS},
        {120.0f, ENEMY_TYPE_PLANT_BOSS},
        {180.0f, ENEMY_TYPE_HORNET_BOSS},
    };
    
    // 普通敌人生成逻辑
    struct SpawnData
    {
        float time;
        float rate;
        EnemyType enemyType;
    };
    
    static SpawnData spawnData[] = {
        {0.0f, 0.7f, ENEMY_TYPE_MOLTEN_MIDGET},
        {30.0f, 0.9f, ENEMY_TYPE_BAT},
        {60.0f, 1.0f, ENEMY_TYPE_PLANT},
        {120.0f, 1.2f, ENEMY_TYPE_MARIO_PLANT},
        {180.0f, 1.5f, ENEMY_TYPE_HORNET}
    };
    
    // 添加游戏关卡逻辑...
    
    // 绘制经验条
    draw_exp_bar();
} 