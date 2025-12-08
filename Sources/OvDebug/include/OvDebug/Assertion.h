/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#pragma once

#include <string>
#include <iostream>
#include <cstdlib>

// C++20 kontrolü ve fallback mekanizması
#if __cplusplus >= 202002L
    #include <source_location>
    namespace ov_sl = std;
#else
    // C++20 öncesi için basit bir source_location implementasyonu
    #include <cstdint>
    struct source_location {
        static constexpr source_location current(
            const char* file = __builtin_FILE(),
            const char* func = __builtin_FUNCTION(),
            std::uint_least32_t line = __builtin_LINE()
        ) noexcept {
            source_location loc;
            loc._file = file;
            loc._function = func;
            loc._line = line;
            return loc;
        }
        
        constexpr const char* file_name() const noexcept { return _file; }
        constexpr const char* function_name() const noexcept { return _function; }
        constexpr std::uint_least32_t line() const noexcept { return _line; }
        constexpr std::uint_least32_t column() const noexcept { return 0; }
        
    private:
        const char* _file = "";
        const char* _function = "";
        std::uint_least32_t _line = 0;
    };
    namespace ov_sl {
        using source_location = ::source_location;
    }
#endif

#ifdef _WIN32
    #include <debugapi.h>
#endif

namespace OvDebug
{
    class Assertion
    {
    public:
        static void Assert(
            bool p_condition, 
            const std::string& p_message = "",
            ov_sl::source_location p_location = ov_sl::source_location::current()
        );
        
        template<typename... Args>
        static void AssertFormat(
            bool p_condition,
            const char* p_format,
            ov_sl::source_location p_location = ov_sl::source_location::current(),
            Args&&... args
        );
        
        [[noreturn]]
        static void Unreachable(
            const std::string& p_message = "",
            ov_sl::source_location p_location = ov_sl::source_location::current()
        );
        
    private:
        static void PrintErrorMessage(
            const std::string& p_message,
            const ov_sl::source_location& p_location
        );
        
        static void PlatformDebugOutput(const std::string& p_message);
    };
}

namespace OvDebug
{
    template<typename... Args>
    void Assertion::AssertFormat(
        bool p_condition,
        const char* p_format,
        ov_sl::source_location p_location,
        Args&&... args)
    {
        char buffer[1024];
        #ifdef _MSC_VER
            sprintf_s(buffer, sizeof(buffer), p_format, std::forward<Args>(args)...);
        #else
            std::snprintf(buffer, sizeof(buffer), p_format, std::forward<Args>(args)...);
        #endif
        
        Assert(p_condition, buffer, p_location);
    }
}