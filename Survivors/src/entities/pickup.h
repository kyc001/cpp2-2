#pragma once
#include "../core/config.h"
#include "../core/common.h" // 用于Vec2定义

// #############################################################
//                      Pickup Definitions
// #############################################################
enum PickupType
{
    PICKUP_TYPE_EXP_BLUE,
    PICKUP_TYPE_EXP_GREEN,
    PICKUP_TYPE_EXP_RED,
    PICKUP_TYPE_BOX
};

struct Pickup
{
    bool triggered;
    float time;
    PickupType type;
    Vec2 pos;
    Vec2 vel;
};
