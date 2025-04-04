#pragma once
#include "../core/assets.h"
#include "../utils/colors.h"
#include "../core/config.h"
#include "../core/common.h" // 添加common.h以确保Vec2和Circle类型可用

// #############################################################
//                      Entity Definitions
// #############################################################
struct Entity
{
    int ID;
    bool boss = false;
    SpriteID spriteID = SPRITE_WHITE;
    Vec2 pos;
    Vec2 desiredDirection;
    Vec2 seperationForce;
    float pushTime;
    Vec2 pushDirection;
    float scale = UNIT_SCALE;
    Vec4 color = COLOR_WHITE;

    Circle collider = {{0.0f, 0.0f}, 20.0f};

    int hp = 5;
    int attack = 10;
    float moveSpeed = 100.0f;
    float attackTime;
    float garlicHitTimer;
    float magmaPuddleHitTimer;
};

struct DamageNumber
{
    int value;
    Vec2 pos;
    float timer;
};
