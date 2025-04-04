#pragma once

// Core 模块
#include "core/common.h"
#include "core/config.h"
#include "core/defines.h"
#include "core/assets.h"
#include "core/gameData.h"
#include "core/game.h"
#include "core/platform.h"
#include "core/input.h"
#include "core/shared.h"

// Entity 模块
#include "entities/entity.h"
#include "entities/player.h"
#include "entities/enemy.h"
#include "entities/pickup.h"
#include "entities/obstacle.h"

// Weapon 模块
#include "weapons/weapon.h"

// Rendering 模块
#include "rendering/render_interface.h"
#include "rendering/custom_gl.h"
#include "rendering/shader_header.h"

// Utils 模块
#include "utils/colors.h"
#include "utils/my_math.h"
#include "utils/logger.h"
#include "utils/sound.h"
#include "utils/easing_functions.h"

// UI 模块
#include "ui/texts.h"

// 这个文件主要用于方便地包含游戏所需的所有头文件
// 使用时只需要包含 "game_module.h" 即可 