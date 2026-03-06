/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>

namespace OvRendering::Utils::Conversions
{
    constexpr uint32_t _log2(float n)
    {
        return ((n < 2) ? 1 : 1 + _log2(n / 2));
    }

    constexpr float Pow2toFloat(uint8_t p_value)
    {
        return static_cast<float>(1U << p_value);
    }

    constexpr uint8_t FloatToPow2(float p_value)
    {
        return static_cast<uint8_t>(_log2(p_value) - 1);
    }
}
