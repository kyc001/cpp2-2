#pragma once
#include "gameData.h"
#include "../rendering/render_interface.h"

// Forward declarations if needed
struct GameState;
struct Input;
struct RenderData;

extern "C"
{
    __declspec(dllexport) void init_game(GameState *gameStateIn, Input *inputIn,
                                         RenderData *renderDataIn);
    __declspec(dllexport) void update_game(GameState *gameStateIn, Input *inputIn,
                                           RenderData *renderDataIn, float dt);
}