/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#include "OvDebug/Assertion.h"
#include <cstdio>
#include <cstdarg>
#include <stdexcept>

namespace OvDebug
{
    void Assertion::Assert(bool p_condition, const std::string& p_message, ov_sl::source_location p_location)
    {
        if (p_condition) 
            return;
        
        PrintErrorMessage(p_message, p_location);
        
        #ifdef _DEBUG
            #ifdef _WIN32
                if (IsDebuggerPresent()) {
                    __debugbreak();
                } else {
                    std::terminate();
                }
            #elif defined(__GNUC__) || defined(__clang__)
                __builtin_trap();
            #else
                std::terminate();
            #endif
        #else
            throw std::logic_error("Assertion failed: " + p_message);
        #endif
    }
    
    // Template fonksiyonun implementasyonu header'da olmalı
    // Bu yüzden burada sadece declaration var
    
    void Assertion::Unreachable(const std::string& p_message, ov_sl::source_location p_location)
    {
        std::string fullMessage = "Unreachable code executed";
        if (!p_message.empty()) {
            fullMessage += ": " + p_message;
        }
        
        PrintErrorMessage(fullMessage, p_location);
        
        #ifdef _DEBUG
            #ifdef _WIN32
                if (IsDebuggerPresent()) {
                    __debugbreak();
                }
            #elif defined(__GNUC__) || defined(__clang__)
                __builtin_trap();
            #endif
        #endif
        
        std::terminate();
    }
    
    void Assertion::PrintErrorMessage(const std::string& p_message, const ov_sl::source_location& p_location)
    {
        std::string errorMessage = 
            "\n"
            "════════════════════════════════════════════════════════════════\n"
            "ASSERTION FAILED\n"
            "════════════════════════════════════════════════════════════════\n"
            "File:     " + std::string(p_location.file_name()) + "\n" +
            "Line:     " + std::to_string(p_location.line()) + "\n" +
            "Function: " + std::string(p_location.function_name()) + "\n";
        
        if (!p_message.empty()) {
            errorMessage += "Message:  " + p_message + "\n";
        }
        
        errorMessage += 
            "════════════════════════════════════════════════════════════════\n\n";
        
        std::cerr << errorMessage;
        PlatformDebugOutput(errorMessage);
    }
    
    void Assertion::PlatformDebugOutput(const std::string& p_message)
    {
        #ifdef _WIN32
            OutputDebugStringA(p_message.c_str());
        #endif
    }
}