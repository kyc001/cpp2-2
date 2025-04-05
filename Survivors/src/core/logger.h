#pragma once

#include "../platform/platform.h"


// Standard Library for sprintf
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

#define KYC_TRACE(msg, ...) _log("TRACE", TEXT_COLOR_GREEN, msg, __VA_ARGS__)
#define KYC_WARN(msg, ...) _log("WARN", TEXT_COLOR_YELLOW, msg, __VA_ARGS__)
#define KYC_ERROR(msg, ...) _log("ERROR", TEXT_COLOR_RED, msg, __VA_ARGS__)
#define KYC_FATAL(msg, ...) _log("FATAL", TEXT_COLOR_LIGHT_RED, msg, __VA_ARGS__)

#ifdef DEBUG
#define KYC_ASSERT(x, msg, ...)              \
{                                          \
if (!(x))                              \
{                                      \
KYC_ERROR(msg, __VA_ARGS__); \
__debugbreak();                    \
}                                      \
}
#else
#define KYC_ASSERT(x, msg, ...) if(!(x)) KYC_ERROR(msg, __VA_ARGS__);
#endif
