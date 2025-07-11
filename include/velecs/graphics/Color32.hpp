/// @file    Color32.hpp
/// @author  Matthew Green
/// @date    2025-07-11 12:42:37
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/math/Vec4.hpp>

#include <glm/vec4.hpp>

#include <cstdint>
#include <iostream>

namespace velecs::graphics {

/// @struct Color32
/// @brief Brief description.
///
/// Rest of description.
struct Color32 {
public:
    // Enums

    // Public Fields

    static const Color32 RED;
    static const Color32 GREEN;
    static const Color32 BLUE;
    static const Color32 CYAN;
    static const Color32 MAGENTA;
    static const Color32 YELLOW;
    static const Color32 GRAY;  // more common US spelling
    static const Color32 GREY;  // more common UK spelling
    static const Color32 WHITE;
    static const Color32 BLACK;
    static const Color32 ORANGE;
    static const Color32 PINK;
    static const Color32 PURPLE;
    static const Color32 BROWN;
    static const Color32 GOLD;
    static const Color32 SILVER;
    static const Color32 BEIGE;
    static const Color32 NAVY;
    static const Color32 MAROON;
    static const Color32 OLIVE;
    static const Color32 INDIGO;
    static const Color32 VIOLET;
    static const Color32 TRANSPARENT;

    uint8_t r; /// @brief The red channel.
    uint8_t g; /// @brief The green channel.
    uint8_t b; /// @brief The blue channel.
    uint8_t a; /// @brief The alpha channel.

    // Constructors and Destructors

    /// @brief Default constructor initializing color to magenta with full alpha.
    Color32();

    /// @brief Copy constructor.
    /// @param[in] color Color32 object to copy.
    Color32(const Color32& color);
    
    /// @brief Default deconstructor.
    ~Color32() = default;

    // Public Methods

    /// @brief Constructs a Color32 object from individual RGB(A) components.
    /// @param[in] r Red channel value (0-255).
    /// @param[in] g Green channel value (0-255).
    /// @param[in] b Blue channel value (0-255).
    /// @param[in] a Alpha channel value (0-255, default is 255 for full opacity).
    /// @return Color32 object with specified RGBA values.
    static Color32 FromUInt8(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255U);

    /// @brief Constructs a Color32 object from a 32-bit unsigned integer.
    /// @details Assumes RGBA order in the 32-bit integer.
    /// @param[in] value 32-bit unsigned integer representing a color.
    /// @return Color32 object with unpacked RGBA values.
    static Color32 FromUInt32(const uint32_t value);

    /// @brief Constructs a Color32 object from individual RGB(A) components in float.
    /// @details Float values should be in the range [0.0, 1.0].
    /// @param[in] r Red channel value (0.0-1.0).
    /// @param[in] g Green channel value (0.0-1.0).
    /// @param[in] b Blue channel value (0.0-1.0).
    /// @param[in] a Alpha channel value (0.0-1.0, default is 1.0 for full opacity).
    /// @return Color32 object with specified RGBA values.
    static Color32 FromFloat(const float r, const float g, const float b, const float a = 1.0f);

    /// @brief Constructs a Color32 object from a hexadecimal color code.
    /// @details Supports the following formats: "#RRGGBBAA", "#RRGGBB", "RRGGBBAA", "RRGGBB".
    ///          If alpha is not specified, it defaults to 255 (fully opaque).
    /// @param[in] hexCode Hexadecimal string representing a color.
    /// @return Color32 object with unpacked RGBA values.
    /// @throws std::invalid_argument if hexCode has an invalid length.
    static Color32 FromHex(const std::string& value);

    static Color32 FromHSV(const uint8_t h, const uint8_t s, const uint8_t v, const uint8_t a = 255U);
    static Color32 FromHSV(const float h, const float s, const float v, const float a = 1.0f);

    /// @brief Converts the Color32 object to a Vec4.
    /// @details Each color channel is normalized to the range [0, 1].
    /// @return glm::vec4 object with RGBA components normalized.
    operator velecs::math::Vec4() const;

    /// @brief Converts the Color32 object to a glm::vec4.
    /// @details Each color channel is normalized to the range [0, 1].
    /// @return glm::vec4 object with RGBA components normalized.
    operator glm::vec4() const;

    /// @brief Overloaded subscript operator for array-like access to color components.
    /// @param[in] index Index of the color component (0 for red, 1 for green, 2 for blue, 3 for alpha).
    /// @return Reference to the specified color component.
    uint8_t& operator[](const std::size_t index);

    /// @brief Const overloaded subscript operator for array-like access to color components.
    /// @param[in] index Index of the color component (0 for red, 1 for green, 2 for blue, 3 for alpha).
    /// @return Const reference to the specified color component.
    const uint8_t& operator[](const std::size_t index) const;

    /// @brief Checks if two Color32 objects are equal
    /// @details Compares all four color channels (r, g, b, a) for exact equality
    /// @param other The Color32 object to compare against
    /// @return true if all channels are equal, false otherwise
    bool operator==(const Color32 other) const;

    /// @brief Checks if two Color32 objects are not equal
    /// @details Returns true if any color channel (r, g, b, a) differs between the two colors
    /// @param other The Color32 object to compare against
    /// @return true if any channel differs, false if all channels are equal
    bool operator!=(const Color32 other) const;

    Color32& operator=(const Color32 other);

    Color32& operator+=(const Color32 other);
    Color32 operator+(const Color32 other) const;
    Color32& operator-=(const Color32 other);
    Color32 operator-(const Color32 other) const;
    Color32& operator*=(const Color32 other);
    Color32 operator*(const Color32 other) const;
    Color32& operator/=(const Color32 other);
    Color32 operator/(const Color32 other) const;

    static Color32 Clamp(const Color32 value, const Color32 min, const Color32 max);
    static Color32 Lerp(const Color32 start, const Color32 end, float t);

    static Color32 AdditiveBlend(const Color32 color1, const Color32 color2);
    static Color32 SubtractiveBlend(const Color32 color1, const Color32 color2);
    static Color32 MultiplyBlend(const Color32 color1, const Color32 color2);
    static Color32 ScreenBlend(const Color32 color1, const Color32 color2);

    static Color32 AlphaBlend(const Color32 src, const Color32 dst);

    void Normalize();
    void ToHSV(float& h, float& s, float& v, float& a) const;

    /// @brief Converts the Color32 to a string with integer values
    /// @return String representation like "RGBA(255, 128, 0, 255)"
    std::string ToString() const;

    /// @brief Converts the Color32 to a string with normalized float values
    /// @return String representation like "RGBA(1.0000, 0.5020, 0.0000, 1.0000)"
    std::string ToStringFloat() const;

    /// @brief Converts the Color32 to a hexadecimal string representation
    /// @param includeAlpha Whether to include the alpha channel in the output (default: true)
    /// @return Hexadecimal string representation like "#FF8000FF" (with alpha) or "#FF8000" (without alpha)
    std::string ToStringHex(bool includeAlpha = true) const;

    friend std::ostream& operator<<(std::ostream& os, const Color32 color);

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Constructor

    /// @brief Constructs a Color32 with specified channel values.
    /// @param[in] r Red channel value.
    /// @param[in] g Green channel value.
    /// @param[in] b Blue channel value.
    /// @param[in] a Alpha channel value (default is 255).
    Color32(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255U);

    // Private Methods
};

std::ostream& operator<<(std::ostream& os, const Color32 color);

} // namespace velecs