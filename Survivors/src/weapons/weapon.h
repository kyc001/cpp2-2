#pragma once
#include "../core/config.h"
#include "../core/assets.h"
#include "../core/common.h" // 添加common.h以确保Vec2类型和Array模板可用

// #############################################################
//                      Weapon Definitions
// #############################################################
enum WeaponID
{
    WEAPON_WHIP,
    WEAPON_GARLIC,
    WEAPON_MAGMA_RING,
    WEAPON_COUNT,
};

struct Weapon
{
    WeaponID ID;
    int level;
    float timePassed;
};

struct ActiveAttack
{
    WeaponID ID;
    int damage;
    float timePassed;

    Vec2 pos;
    Vec2 targetPos;

    union
    {
        struct
        {
            int maxSlashCount;
            int currentSlashCount;
        } whip;
    };
};

struct DamagingArea
{
    WeaponID weaponID;
    SpriteID spriteID;
    float timePassed;
    float duration;
    int damage;
    Vec2 pos;
    Vec2 size;

    Array<int, MAX_ENEMIES> hitEnemyIDs;
};
