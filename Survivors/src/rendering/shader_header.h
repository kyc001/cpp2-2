#ifdef GAME // Inside Game
#pragma once

#include "../utils/my_math.h"
#define vec2 Vec2
#define ivec2 IVec2
#define vec4 Vec4
typedef int RenderOptions;

#else // Inside Shader

// #define BIT(i) 1 << i // Commented out redundant BIT macro

#endif

#define RENDER_OPTION_FLIP_X BIT(0)
#define RENDER_OPTION_FLIP_Y BIT(1)
#define RENDER_OPTION_FONT BIT(2)
#define RENDER_OPTION_TOP_LEFT BIT(3)
#define RENDER_OPTION_GRAYSCALE_STUFF_DUNNO BIT(4)

struct Transform
{
    vec2 pos;
    vec2 size;
    ivec2 atlasOffset;
    ivec2 spriteSize;
    int renderOptions;
    int materialIdx;
};

struct Material
{
    vec4 color;
};

#define MAX_MATERIALS 100
#define LIGHT_COUNT 5

struct Light