#pragma once
#include <cstdint>

#define EPSILON 0.000001f

#define STATIC static
#define PERSIST static
#define GLOBAL static
#define internal static

#define KB(x) ((uint64_t)1024 * x)
#define MB(x) ((uint64_t)1024 * KB(x))
#define GB(x) ((uint64_t)1024 * MB(x))

#define ARRAY_SIZE(arr) sizeof((arr)) / sizeof((arr)[0])

#define BIT(x) (1u << (x))
#define FOURCC(str) (uint32_t)(((uint32_t)str[0]) | ((uint32_t)str[1] << 8) | ((uint32_t)str[2] << 16) | ((uint32_t)str[3] << 24))

#ifdef DEBUG
#define ASSERT(cond, msg) if(!(cond)) { __debugbreak(); }
#else
#define ASSERT(cond, msg)
#endif

/* 默认switch处理宏 */
#define INVALID_DEFAULT_CASE  \
    default:                  \
    {                         \
        ASSERT(false, "");    \
    }