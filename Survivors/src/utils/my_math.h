#pragma once

#include <math.h>
#include "../core/defines.h"

// 修改STATIC宏为internal，与示例保持一致
#ifndef STATIC
#define STATIC internal
#endif

struct Vec2
{
    float x;
    float y;
};

struct IVec2
{
    int x;
    int y;
};

struct Vec4
{
    float x;
    float y;
    float z;
    float w;
};

struct Rect
{
    Vec2 pos;
    Vec2 size;
};

struct Circle
{
    Vec2 pos;
    float radius;
};

STATIC Vec2 operator+(Vec2 a, Vec2 other)
{
    Vec2 result;
    result.x = a.x + other.x;
    result.y = a.y + other.y;
    return result;
}

STATIC Vec2 operator+(Vec2 a, float scalar)
{
    return Vec2{
        a.x + scalar,
        a.y + scalar};
}

STATIC Vec2 &operator+=(Vec2 &a, Vec2 b)
{
    a = a + b;
    return a;
}

STATIC Vec2 operator-(Vec2 a, Vec2 other)
{
    return Vec2{
        a.x - other.x,
        a.y - other.y};
}

STATIC Vec2 &operator-=(Vec2 &a, Vec2 b)
{
    a = a - b;
    return a;
}

STATIC Vec2 operator*(Vec2 a, Vec2 other)
{
    return Vec2{
        a.x * other.x,
        a.y * other.y};
}

STATIC Vec2 operator*(Vec2 a, float scalar)
{
    return Vec2{
        a.x * scalar,
        a.y * scalar};
}

STATIC Vec2 operator/(Vec2 a, Vec2 other)
{
    return Vec2{
        a.x / other.x,
        a.y / other.y};
}

STATIC Vec2 operator/(Vec2 a, float scalar)
{
    return Vec2{
        a.x / scalar,
        a.y / scalar};
}

STATIC Vec2 vec_2(float x, float y)
{
    return {x, y};
}

STATIC Vec2 vec_2(float val)
{
    return {val, val};
}

STATIC IVec2 operator+(IVec2 a, IVec2 other)
{
    return IVec2{
        a.x + other.x,
        a.y + other.y};
}

STATIC IVec2 operator-(IVec2 a, IVec2 other)
{
    return IVec2{
        a.x - other.x,
        a.y - other.y};
}

STATIC IVec2 operator/(IVec2 a, IVec2 other)
{
    return IVec2{
        a.x / other.x,
        a.y / other.y};
}

STATIC IVec2 operator/(IVec2 a, int scalar)
{
    return IVec2{
        a.x / scalar,
        a.y / scalar};
}

STATIC IVec2 ivec_2(int val)
{
    return {val, val};
}

STATIC bool operator==(Vec4 a, Vec4 b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

STATIC Vec4 operator*(Vec4 a, Vec4 other)
{
    return Vec4{
        a.x * other.x,
        a.y * other.y,
        a.z * other.z,
        a.w * other.w};
}

STATIC Vec4 operator*(Vec4 a, float scalar)
{
    return Vec4{
        a.x * scalar,
        a.y * scalar,
        a.z * scalar,
        a.w * scalar};
}

STATIC Vec4 &operator*=(Vec4 &a, Vec4 b)
{
    a = a * b;
    return a;
}

STATIC Vec4 &operator*=(Vec4 &a, float scalar)
{
    a = a * scalar;
    return a;
}

STATIC float clamp(float value, float min, float max)
{
    if (value > max)
    {
        value = max;
    }
    else if (value < min)
    {
        value = min;
    }

    return value;
}

STATIC int clamp(int value, int min, int max)
{
    if (value > max)
    {
        value = max;
    }
    else if (value < min)
    {
        value = min;
    }

    return value;
}

STATIC Vec2 vec_2(IVec2 v)
{
    return {(float)v.x, (float)v.y};
}

STATIC Vec2 vec_2(int number)
{
    return {(float)number, (float)number};
}

STATIC float length(Vec2 v)
{
    return (float)sqrt((v.x * v.x) + (v.y * v.y));
}

STATIC float length_squared(Vec2 v)
{
    return (v.x * v.x) + (v.y * v.y);
}

STATIC Vec2 normalize(Vec2 v)
{
    float vecLength = length(v);
    if (vecLength <= 0)
        return Vec2{0.0f, 0.0f};

    return Vec2{
        v.x / vecLength,
        v.y / vecLength};
}

STATIC float sinf2(float t)
{
    return sinf(t) * 0.5f + 0.5f;
}

STATIC bool point_in_circle(Vec2 point, Circle c)
{
    Vec2 pointToOrigen = c.pos - point;
    float lengthSquared = length_squared(pointToOrigen);
    return lengthSquared <= c.radius * c.radius;
}

STATIC bool point_in_rect(Vec2 point, Rect rect)
{
    return (point.x >= rect.pos.x &&
            point.x <= rect.pos.x + rect.size.x &&
            point.y >= rect.pos.y &&
            point.y <= rect.pos.y + rect.size.y);
}

STATIC bool circle_collision(Circle a, Circle b, float *pushout = 0)
{
    bool result = false;

    float distanceSquared = length_squared(b.pos - a.pos);
    if (distanceSquared < (a.radius + b.radius) * (a.radius + b.radius))
    {
        if (pushout)
        {
            *pushout = (float)sqrt(distanceSquared) - a.radius - b.radius;
        }
        result = true;
    }

    return result;
}

STATIC bool rect_circle_collision(Rect rect, Circle c, Vec2 *pushout)
{
    Vec2 topLeft = rect.pos;
    Vec2 topRight = rect.pos + Vec2{rect.size.x};
    Vec2 bottomLeft = rect.pos + Vec2{0.0f, rect.size.y};
    Vec2 bottomRight = rect.pos + rect.size;

    Vec2 projectedPos = {};
    projectedPos.y = clamp(c.pos.y, topLeft.y, bottomRight.y);
    projectedPos.x = clamp(c.pos.x, topLeft.x, bottomRight.x);

    if (point_in_circle(projectedPos, c))
    {
        if (pushout)
        {
            Vec2 pointDir = c.pos - projectedPos;
            float lengthToPoint = length(pointDir);
            float penetrationLength = c.radius - lengthToPoint;

            Vec2 pushoutDir = normalize(pointDir);

            *pushout = pushoutDir * penetrationLength;
        }
        return true;
    }

    return false;
}

template <typename T>
STATIC T max(T a, T b)
{
    return a > b ? a : b;
}

template <typename T>
STATIC T min(T a, T b)
{
    return a < b ? a : b;
}

STATIC int min(int a, int b)
{
    return a < b ? a : b;
}

STATIC int max(int a, int b)
{
    return a > b ? a : b;
}
