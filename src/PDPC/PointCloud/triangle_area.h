#pragma once

namespace pdpc {

template<typename T>
T triangle_area(T x1, T y1, T x2, T y2, T x3, T y3)
{
    const T ux = x2 - x1;
    const T uy = y2 - y1;
    const T vx = x3 - x1;
    const T vy = y3 - y1;

    return T(0.5) * (ux*vy - uy*vx);
}

} // namespace pdpc
