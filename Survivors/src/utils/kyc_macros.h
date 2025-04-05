#pragma once
#include <stdlib.h>          // 用于malloc、free等
#include <string.h>          // 用于memset
#include "../utils/logger.h" // 包含logger.h以使用日志函数

/**
 * KYC宏定义库
 * 用于简化常用C++操作和提高代码可读性
 */

// 通用宏定义
#define KYC_UNUSED(x) (void)(x)
#define KYC_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// 字符串操作宏
#define KYC_STR(x) #x
#define KYC_XSTR(x) KYC_STR(x)

// 内存操作宏
#define KYC_ALLOC(size) malloc(size)
#define KYC_FREE(ptr) free(ptr)
#define KYC_ZERO_MEM(ptr, size) memset(ptr, 0, size)

// 调试和断言宏
#ifdef _DEBUG
#define KYC_DEBUG_ONLY(x) x
#else
#define KYC_DEBUG_ONLY(x)
#endif

// 使用logger.h中的错误处理函数
#ifdef ERROR
#define KYC_ERROR(msg, ...) ERROR(msg, ##__VA_ARGS__)
#else
#define KYC_ERROR(msg, ...) _log("ERROR", TEXT_COLOR_RED, msg, ##__VA_ARGS__)
#endif

#define KYC_ASSERT(condition, msg, ...)    \
    do                                     \
    {                                      \
        if (!(condition))                  \
        {                                  \
            KYC_ERROR(msg, ##__VA_ARGS__); \
            __debugbreak();                \
        }                                  \
    } while (0)

// 额外的宏用于简化代码
#define KYC_MIN(a, b) ((a) < (b) ? (a) : (b))
#define KYC_MAX(a, b) ((a) > (b) ? (a) : (b))
#define KYC_CLAMP(x, min, max) KYC_MIN(KYC_MAX(x, min), max)
