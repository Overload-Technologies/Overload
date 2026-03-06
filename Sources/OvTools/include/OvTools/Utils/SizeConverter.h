/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <tuple>
#include <string>


namespace OvTools::Utils
{
    /*
    * Handles size conversions
    */
    class SizeConverter
    {
    public:
        enum class ESizeUnit
        {
            BYTE        = 0,
            KILO_BYTE   = 3,
            MEGA_BYTE   = 6,
            GIGA_BYTE   = 9,
            TERA_BYTE   = 12
        };

        /**
        * Disabled constructor
        */
        SizeConverter() = delete;

        /**
        * Converts the given size to the optimal unit to avoid large numbers (Ex: 1000B will returns 1KB)
        * @param p_value
        * @param p_unit
        */
        static std::pair<float, ESizeUnit> ConvertToOptimalUnit(float p_value, ESizeUnit p_unit);

        /**
        * Converts the given size from one unit to another
        * @param p_value
        * @param p_from
        * @param p_to
        */
        static float Convert(float p_value, ESizeUnit p_from, ESizeUnit p_to);

        /**
        * Converts the given unit to a string
        * @param p_unit
        */
        static std::string UnitToString(ESizeUnit p_unit);
    };
}