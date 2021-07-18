#pragma once

#include "Common/Common.h"

#include "Math/Math.h"
#include "Math/Vector2.h"

struct Rectangle
{
    float x;
    float y;
    float w;
    float h;

    Rectangle()
        : x(0.0f)
        , y(0.0f)
        , w(0.0f)
        , h(0.0f)
    {

    }

    Rectangle(const Rectangle& other)
        : x(other.x)
        , y(other.y)
        , w(other.w)
        , h(other.h)
    {

    }

    Rectangle(float inX, float inY, float inW, float inH)
        : x(inX)
        , y(inY)
        , w(inW)
        , h(inH)
    {

    }

    FORCEINLINE void Set(float inX, float inY, float inW, float inH)
    {
        x = inX;
        y = inY;
        w = inW;
        h = inH;
    }

    FORCEINLINE float Left() const
    {
        return x;
    }

    FORCEINLINE float Right() const
    {
        return x + w;
    }

    FORCEINLINE float Bottom() const
    {
        return y + h;
    }

    FORCEINLINE float Top() const
    {
        return y;
    }

    FORCEINLINE bool Contains(float px, float py) const
    {
        return px >= x && px <= x + w && py >= y && py <= y + h;
    }

    FORCEINLINE bool Contains(const Vector2& p) const
    {
        return Contains(p.x, p.y);
    }

    FORCEINLINE bool Contains(const Rectangle& r) const
    {
        return Left() <= r.Left() &&
               Right() >= r.Right() &&
               Top() <= r.Top() &&
               Bottom() >= r.Bottom();
    }
};
