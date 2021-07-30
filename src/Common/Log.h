#pragma once

#include "Common/Common.h"

#include <stdio.h>
#include <stdarg.h>

struct LogTrace
{
	enum
	{
		MAX_STRING_SIZE = 4096
	};

	enum Type
	{
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	static int32 GetVarArgs(ANSICHAR* dest, int32 destSize, const ANSICHAR*& fmt, va_list argPtr);

	static int32 FormatString(ANSICHAR* dest, const ANSICHAR* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		int32 result = GetVarArgs(dest, MAX_STRING_SIZE, fmt, ap);
		if (result >= MAX_STRING_SIZE)
		{
			result = -1;
		}
		va_end(ap);

		return result;
	}

	template <typename FmtType, typename... Types>
	static void LogMessage(Type type, const FmtType& fmt, Types... args)
	{
		ANSICHAR buffer[MAX_STRING_SIZE];
		int32 index = FormatString(buffer, fmt, args...);
		if (index > -1)
		{
			buffer[index] = '\0';
		}

		LogToConsole(type, buffer);
	}
	
	static void LogToConsole(Type type, const ANSICHAR* msg);
};

#if defined(DEBUG) || defined(_DEBUG)
	#define LOGD(...)  do { LogTrace::LogMessage(LogTrace::Debug, __VA_ARGS__); } while (0)
	#define LOGI(...)  do { LogTrace::LogMessage(LogTrace::Info,  __VA_ARGS__); } while (0)
#else
	#define LOGD(...)  do { } while (0)
	#define LOGI(...)  do { } while (0)
#endif

#define LOGW(...)  do { LogTrace::LogMessage(LogTrace::Warning, __VA_ARGS__); } while (0)
#define LOGE(...)  do { LogTrace::LogMessage(LogTrace::Error,   __VA_ARGS__); } while (0)
#define LOGF(...)  do { LogTrace::LogMessage(LogTrace::Fatal,   __VA_ARGS__); } while (0)
