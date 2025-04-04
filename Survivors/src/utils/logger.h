#pragma once

#include "../core/platform.h"

// 用于sprintf的标准库
#include <stdio.h>

template <typename... Args>
void _log(char *prefix, TextColor color, char *msg, Args... args)
{
    char fmtBuffer[32000] = {};
    char msgBuffer[32000] = {};
    sprintf(fmtBuffer, "%s: %s\n", prefix, msg);
    sprintf(msgBuffer, fmtBuffer, args...);
    platform_log(msgBuffer, color);
}

#define TRACE(msg, ...) _log("TRACE", TEXT_COLOR_GREEN, msg, __VA_ARGS__)
#define WARN(msg, ...) _log("WARN", TEXT_COLOR_YELLOW, msg, __VA_ARGS__)
#define ERROR(msg, ...) _log("ERROR", TEXT_COLOR_RED, msg, __VA_ARGS__)
#define FATAL(msg, ...) _log("FATAL", TEXT_COLOR_LIGHT_RED, msg, __VA_ARGS__)

#ifdef DEBUG
#define ASSERT(x, msg, ...)          \
    {                                \
        if (!(x))                    \
        {                            \
            ERROR(msg, __VA_ARGS__); \
            __debugbreak();          \
        }                            \
    }
#else
#define ASSERT(x, msg, ...) \
    if (!(x))               \
        ERROR(msg, __VA_ARGS__);
#endif
