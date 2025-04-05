#pragma once
#include "../core/gameData.h" // Include gameData.h for PlayerType and Player
#include "../core/assets.h"
#include "../core/input.h"
#include "../core/common.h"
#include "../weapons/weapon.h" // Added include for Weapon type used in Player struct (via gameData.h)

// #############################################################
//                      Player Definitions
// #############################################################
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
