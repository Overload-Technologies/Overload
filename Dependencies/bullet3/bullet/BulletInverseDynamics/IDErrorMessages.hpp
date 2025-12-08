///@file error message utility functions
#ifndef IDUTILS_HPP_
#define IDUTILS_HPP_
#include <cstring>
/// name of file being compiled, without leading path components
#include <source_location>
#include <string_view>

#define InvdynFileName \
    []() constexpr -> std::string_view { \
        constexpr std::source_location loc = std::source_location::current(); \
        constexpr std::string_view path = loc.file_name(); \
        constexpr size_t pos = path.find_last_of("/\\"); \
        if constexpr (pos == std::string_view::npos) { \
            return path; \
        } else { \
            constexpr std::string_view result = path.substr(pos + 1); \
            return result; \
        } \
    }()
#if !defined(BT_ID_WO_BULLET) && !defined(BT_USE_INVERSE_DYNAMICS_WITH_BULLET2)
#include "Bullet3Common/b3Logging.h"
#define bt_id_error_message(...) b3Error(__VA_ARGS__)
#define bt_id_warning_message(...) b3Warning(__VA_ARGS__)
#define id_printf(...) b3Printf(__VA_ARGS__)
#else  // BT_ID_WO_BULLET
#include <cstdio>
/// print error message with file/line information
#include <cstdio>
#include <source_location>
#define bt_id_error_message(...)                                             \
	do                                                                       \
	{                                                                        \
		const auto loc = std::source_location::current();					 \	 
		fprintf(stderr, "[Error:%s:%d] ", _loc.file_name(), _loc.line());    \
		fprintf(stderr, __VA_ARGS__);                                        \
	} while (0)
/// print warning message with file/line information
#define bt_id_warning_message(...)                                             \
	do                                                                         \
	{                                                                          \
		const auto loc = std::source_location::current(); 					   \
		fprintf(stderr, "[Warning:%s:%d] ", loc.file_name(), loc.line());  	   \
		fprintf(stderr, __VA_ARGS__);                                          \
	} while (0)
#define id_printf(...) printf(__VA_ARGS__)
#endif  // BT_ID_WO_BULLET
#endif