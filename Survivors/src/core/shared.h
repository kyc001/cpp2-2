#pragma once
#include "../game/gameData.h"
#include "../graphics/render_interface.h"
#include "../input/input.h"

extern "C"
{
  __declspec(dllexport) void init_game(GameState* gameStateIn, Input* inputIn,
                                       RenderData* renderDataIn);
  __declspec(dllexport) void update_game(GameState* gameStateIn, Input* inputIn, 
                                         RenderData* renderDataIn, float dt);
}