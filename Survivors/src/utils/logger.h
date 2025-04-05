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

#define TRACE(msg, ...) _log("TRACE", TEXT_COLOR_GREEN, msg, ##__VA_ARGS__)
#define WARN(msg, ...) _log("WARN", TEXT_COLOR_YELLOW, msg, ##__VA_ARGS__)

// 检查是否已在wingdi.h中定义了ERROR宏
#if defined(ERROR) && !defined(LOGGER_ERROR_DEFINED)
#define LOGGER_ERROR_DEFINED
#undef ERROR // 取消wingdi.h的ERROR定义
#endif

#define ERROR(msg, ...) _log("ERROR", TEXT_COLOR_RED, msg, ##__VA_ARGS__)
#define KYC_FATAL(msg, ...) _log("FATAL", TEXT_COLOR_LIGHT_RED, msg, ##__VA_ARGS__)

#ifdef DEBUG
#ifndef KYC_ASSERT
#define KYC_ASSERT(x, msg, ...)          \
    {                                \
        if (!(x))                    \
        {                            \
            ERROR(msg, ##__VA_ARGS__); \
            __debugbreak();          \
        }                            \
    }
#else
// 如果ASSERT已被定义，使用不同名称
#define LOGGER_ASSERT(x, msg, ...)          \
    {                                       \
        if (!(x))                           \
        {                                   \
            LOGGER_ERROR(msg, ##__VA_ARGS__); \
            __debugbreak();                 \
        }                                   \
    }
#endif
#else
#ifndef KYC_ASSERT
#define KYC_ASSERT(x, msg, ...) \
    if (!(x))               \
        ERROR(msg, ##__VA_ARGS__);
#else
// 如果ASSERT已被定义，使用不同名称
#define LOGGER_ASSERT(x, msg, ...) \
    if (!(x))                      \
        LOGGER_ERROR(msg, ##__VA_ARGS__);
#endif
#endif
