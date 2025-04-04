#pragma once
#include "../core/assets.h"
#include "../core/config.h"
#include "../weapons/weapon.h"
#include "../core/common.h" // 添加common.h以确保Vec2和Circle类型可用

// #############################################################
//                      Player Definitions
// #############################################################
enum PlayerType
{
    PLAYER_TYPE_BELMOT,
    PLAYER_TYPE_GANDALF,
    PLAYER_TYPE_WHOSWHO,
};

struct Player
{
    PlayerType type;
    SpriteID spriteID = SPRITE_WHITE;
    Vec2 pos;
    Circle collider = {{0.0f, 0.0f}, 12.0f};
    float speed = 400.0f;
    float pickupTriggerRadius = 60.0f;
    float pickupRadius = 20.0f;
    bool flipX;

    int level = 1;
    int maxHP = 300;
    int hp = 300;
    int exp;

    Array<Weapon, WEAPON_COUNT> weapons;
};
