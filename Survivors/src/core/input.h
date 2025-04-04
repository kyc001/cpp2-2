#pragma once
#include "../utils/my_math.h"
#include <windows.h>

enum KeyID
{
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,

    KEY_A = 'A',
    KEY_B = 'B',
    KEY_C = 'C',
    KEY_D = 'D',
    KEY_E = 'E',
    KEY_F = 'F',
    KEY_G = 'G',
    KEY_H = 'H',
    KEY_I = 'I',
    KEY_J = 'J',
    KEY_K = 'K',
    KEY_L = 'L',
    KEY_M = 'M',
    KEY_N = 'N',
    KEY_O = 'O',
    KEY_P = 'P',
    KEY_Q = 'Q',
    KEY_R = 'R',
    KEY_S = 'S',
    KEY_T = 'T',
    KEY_U = 'U',
    KEY_V = 'V',
    KEY_W = 'W',
    KEY_X = 'X',
    KEY_Y = 'Y',
    KEY_Z = 'Z',
    KEY_ENTER = VK_RETURN,
    KEY_ESC = VK_ESCAPE,
    KEY_SPACE = VK_SPACE,
    KEY_SHIFT = VK_SHIFT,
    KEY_TAB = VK_TAB,
    KEY_CONTROL = VK_CONTROL,
    KEY_ALT = VK_MENU,
    KEY_BACKSPACE = VK_BACK,
    KEY_DELETE = VK_DELETE,
    KEY_LEFT_MOUSE = VK_LBUTTON,
    KEY_RIGHT_MOUSE = VK_RBUTTON,
    KEY_MIDDLE_MOUSE = VK_MBUTTON,
    KEY_0 = '0',
    KEY_1 = '1',
    KEY_2 = '2',
    KEY_3 = '3',
    KEY_4 = '4',
    KEY_5 = '5',
    KEY_6 = '6',
    KEY_7 = '7',
    KEY_8 = '8',
    KEY_9 = '9',

    KEY_COUNT = 256,
};

struct ButtonState
{
    int halfTransitionCount;
    bool isDown;
};

struct Input
{
    ButtonState keys[KEY_COUNT];
    Vec2 mousePosScreen;
    Vec2 oldMousePos;
    Vec2 relMouseScreen;

    Vec2 screenSize;
};

GLOBAL Input *input = 0;

STATIC bool is_key_down(KeyID key)
{
    return input->keys[key].isDown;
}

STATIC bool is_key_up(KeyID key)
{
    return !input->keys[key].isDown;
}

STATIC bool is_key_pressed(KeyID key)
{
    return input->keys[key].halfTransitionCount > 0 && input->keys[key].isDown;
}

STATIC bool is_key_released(KeyID key)
{
    return input->keys[key].halfTransitionCount > 0 && !input->keys[key].isDown;
}
