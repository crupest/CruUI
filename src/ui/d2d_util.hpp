#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "pre.hpp"

#include "system_headers.hpp"

#include "ui_base.hpp"

namespace cru::ui
{
    inline D2D1_POINT_2F Convert(const Point& point)
    {
        return D2D1::Point2F(point.x, point.y);
    }

    inline D2D1_RECT_F Convert(const Rect& rect)
    {
        return D2D1::RectF(rect.left, rect.top, rect.left + rect.width, rect.top + rect.height);
    }

    inline Point Convert(const D2D1_POINT_2F& point)
    {
        return Point(point.x, point.y);
    }

    inline Rect Convert(const D2D1_RECT_F& rect)
    {
        return Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    }

    inline bool operator==(const D2D1_POINT_2F& left, const D2D1_POINT_2F& right)
    {
        return left.x == right.x && left.y == right.y;
    }

    inline bool operator!=(const D2D1_POINT_2F& left, const D2D1_POINT_2F& right)
    {
        return !(left == right);
    }

    inline bool operator==(const D2D1_RECT_F& left, const D2D1_RECT_F& right)
    {
        return left.left == right.left &&
            left.top == right.top &&
            left.right == right.right &&
            left.bottom == right.bottom;
    }

    inline bool operator!=(const D2D1_RECT_F& left, const D2D1_RECT_F& right)
    {
        return !(left == right);
    }

    inline bool operator==(const D2D1_ROUNDED_RECT& left, const D2D1_ROUNDED_RECT& right)
    {
        return left.rect == right.rect && left.radiusX == right.radiusX && left.radiusY == right.radiusY;
    }

    inline bool operator!=(const D2D1_ROUNDED_RECT& left, const D2D1_ROUNDED_RECT& right)
    {
        return !(left == right);
    }
}