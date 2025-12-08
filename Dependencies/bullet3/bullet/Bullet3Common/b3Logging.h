
#ifndef B3_LOGGING_H
#define B3_LOGGING_H

#include <cstdarg>
#include <source_location>

#ifdef __cplusplus
extern "C"
{
#endif

///We add the do/while so that the statement "if (condition) b3Printf("test"); else {...}" would fail
///You can also customize the message by uncommenting out a different line below
#define b3Printf(...) b3OutputPrintfVarArgsInternal(std::source_location::current(), __VA_ARGS__)
	/*#define b3Printf(...) do{
	auto _b3_loc = std::source_location::current();
	b3OutputWarningMessageVarArgsInternal(_b3_loc, "b3Warning[%s:%d in %s]: ", _b3_loc.file_name(), _b3_loc.line(), _b3_loc.function_name());
	b3OutputWarningMessageVarArgsInternal(_b3_loc, __VA_ARGS__);
} while(0); */
	//#define b3Printf b3OutputPrintfVarArgsInternal
	//#define b3Printf(...) printf(_b3_loc, __VA_ARGS__)
	//#define b3Printf(...)
#define b3Warning(...) do{
	auto _b3_loc = std::source_location::current();
	b3OutputWarningMessageVarArgsInternal(_b3_loc, "b3Warning[%s:%d in %s]: ", _b3_loc.file_name(), _b3_loc.line(), _b3_loc.function_name());
	b3OutputWarningMessageVarArgsInternal(_b3_loc, __VA_ARGS__);
} while(0);
#define b3Error(...)do{
	auto _b3_loc = std::source_location::current();
	b3OutputErrorMessageVarArgsInternal(_b3_loc, "b3Error[%s:%d in %s]: ", _b3_loc.file_name(), _b3_loc.line(), _b3_loc.function_name());
	b3OutputErrorMessageVarArgsInternal(_b3_loc, __VA_ARGS__);
} while (0)
#ifndef B3_NO_PROFILE

	void b3EnterProfileZone(const char* name, const std::source_location& location = std::source_location::current());
	void b3LeaveProfileZone();
#ifdef __cplusplus

	class b3ProfileZone
	{
	public:
		b3ProfileZone(const char* name, const std::source_location& location = std::source_location::current())
		{
			b3EnterProfileZone(name);
		}

		~b3ProfileZone()
		{
			b3LeaveProfileZone();
		}
	};

#define B3_PROFILE(name) b3ProfileZone __profile(name, std::source_location::current());
#endif

#else  //B3_NO_PROFILE

#define B3_PROFILE(name)
#define b3StartProfile(a)
#define b3StopProfile

#endif  //#ifndef B3_NO_PROFILE

	typedef void(b3PrintfFunc)(const char* msg, const std::source_location& location);
	typedef void(b3WarningMessageFunc)(const char* msg, const std::source_location& location);
	typedef void(b3ErrorMessageFunc)(const char* msg, const std::source_location& location);
	typedef void(b3EnterProfileZoneFunc)(const char* msg, const std::source_location& location);
	typedef void(b3LeaveProfileZoneFunc)();

	///The developer can route b3Printf output using their own implementation
	void b3SetCustomPrintfFunc(b3PrintfFunc* printfFunc);
	void b3SetCustomWarningMessageFunc(b3WarningMessageFunc* warningMsgFunc);
	void b3SetCustomErrorMessageFunc(b3ErrorMessageFunc* errorMsgFunc);

	///Set custom profile zone functions (zones can be nested)
	void b3SetCustomEnterProfileZoneFunc(b3EnterProfileZoneFunc* enterFunc);
	void b3SetCustomLeaveProfileZoneFunc(b3LeaveProfileZoneFunc* leaveFunc);

	///Don't use those internal functions directly, use the b3Printf or b3SetCustomPrintfFunc instead (or warning/error version)
	void b3OutputPrintfVarArgsInternal(const std::source_location& location , const char* str, ...);
	void b3OutputWarningMessageVarArgsInternal(const std::source_location& location, const char* str, ...);
	void b3OutputErrorMessageVarArgsInternal(const std::source_location& location, const char* str, ...);

#ifdef __cplusplus
}
#endif

#endif  //B3_LOGGING_H