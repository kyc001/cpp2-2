#pragma once
#include "defines.h"

#ifdef DEBUG
#include <stdlib.h>
#include <stdio.h>
#define assert(cond)                                              \
    if (!(cond))                                                  \
    {                                                             \
        __debugbreak();                                           \
        printf("ASSERT FAILED AT %s (%i)\n", __FILE__, __LINE__); \
        exit(1);                                                  \
    }
#else
#define assert(cond)
#endif

template <typename T, int N>
struct Array
{
    static constexpr int maxElements = N;

    int count = 0;
    T elements[N];

    T &operator[](int idx)
    {
        assert(idx >= 0);
        assert(idx < count);
        return elements[idx];
    }

    int add(T element)
    {
        assert(count < maxElements);
        elements[count] = element;
        return count++;
    }

    void remove_idx_and_swap(int idx)
    {
        assert(idx >= 0);
        assert(idx < count);
        elements[idx] = elements[--count];
    }

    void clear()
    {
        count = 0;
    }

    bool is_full()
    {
        return count == N;
    }
};