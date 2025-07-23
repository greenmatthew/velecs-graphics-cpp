/// @file    Rect.hpp
/// @author  Matthew Green
/// @date    2025-07-11 17:34:58
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/Math/Vec2.hpp>

namespace velecs::graphics {

/// @struct Rect
/// @brief Represents a 2D rectangle defined by minimum and maximum points.
///
/// This class encapsulates a 2D rectangle defined by two points: the minimum point (bottom-left) and the maximum point (top-right).
/// It provides constructors for specifying the rectangle's dimensions and utility methods for working with the rectangle.
struct Rect {
public:
    // Enums

    // Public Fields

    using Vec2 = velecs::math::Vec2;

    Vec2 min; /// @brief Minimum point (bottom-left corner) of the rectangle.
    Vec2 max; /// @brief Maximum point (top-right corner) of the rectangle.

    // Constructors and Destructors
    
    /// @brief Constructs a rectangle with the specified minimum and maximum points.
    /// 
    /// @param min The minimum point (bottom-left corner) of the rectangle.
    /// @param max The maximum point (top-right corner) of the rectangle.
    inline Rect(const Vec2 min, const Vec2 max)
        : min(min), max(max) {}

    /// @brief Constructs a rectangle with the specified dimensions.
    /// 
    /// @param left The x-coordinate of the bottom-left corner of the rectangle.
    /// @param bottom The y-coordinate of the bottom-left corner of the rectangle.
    /// @param right The x-coordinate of the top-right corner of the rectangle.
    /// @param top The y-coordinate of the top-right corner of the rectangle.
    inline Rect(const float left, const float bottom, const float right, const float top)
        : min(Vec2{ left, bottom }), max(Vec2{ right, top }) {}
    
    /// @brief Default deconstructor.
    ~Rect() = default;

    // Public Methods

    /// @brief Computes and returns the width of the rectangle.
    ///
    /// This method calculates the width of the rectangle by subtracting the x-coordinate of the minimum point from the x-coordinate of the maximum point.
    /// @return The width of the rectangle.
    inline float GetWidth() const
    {
        return max.x - min.x;
    }

    /// @brief Computes and returns half the width of the rectangle.
    ///
    /// This method calculates half the width of the rectangle by subtracting the x-coordinate 
    /// of the minimum point from the x-coordinate of the maximum point, and then dividing the result by 2.
    /// @return Half the width of the rectangle.
    inline float GetHalfWidth() const
    {
        return GetWidth() * 0.5f;
    }

    /// @brief Computes and returns the height of the rectangle.
    ///
    /// This method calculates the height of the rectangle by subtracting the y-coordinate of the minimum point from the y-coordinate of the maximum point.
    /// @return The height of the rectangle.
    inline float GetHeight() const
    {
        return max.y - min.y;
    }

    /// @brief Computes and returns half the height of the rectangle.
    ///
    /// This method calculates half the height of the rectangle by subtracting the y-coordinate 
    /// of the minimum point from the y-coordinate of the maximum point, and then dividing the result by 2.
    /// @return Half the height of the rectangle.
    inline float GetHalfHeight() const
    {
        return GetHeight() * 0.5f;
    }

    /// @brief Computes and returns the area of the rectangle.
    /// @return The area of the rectangle.
    inline float GetArea() const
    {
        return GetWidth() * GetHeight();
    }

    /// @brief Computes and returns the perimeter of the rectangle.
    /// @return The perimeter of the rectangle.
    inline float GetPerimeter() const
    {
        return 2 * (GetWidth() + GetHeight());
    }

    /// @brief Computes and returns the center point of the rectangle.
    /// @return The center point of the rectangle.
    inline Vec2 GetCenter() const
    {
        return Vec2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
    }

    /// @brief Checks if a point is within the rectangle.
    /// @param[in] point The point to check.
    /// @return True if the point is within the rectangle, false otherwise.
    inline bool Contains(const Vec2& point) const
    {
        return point.x >= min.x && point.x <= max.x &&
            point.y >= min.y && point.y <= max.y;
    }

    /// @brief Checks if another rectangle intersects with this rectangle using AABB intersection test.
    /// @param[in] other The other rectangle to check.
    /// @return True if the rectangles intersect, false otherwise.
    /// @note Uses Axis-Aligned Bounding Box (AABB) intersection detection
    inline bool Intersects(const Rect& other) const
    {
        return max.x >= other.min.x && min.x <= other.max.x &&
            max.y >= other.min.y && min.y <= other.max.y;
    }

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace velecs::graphics
