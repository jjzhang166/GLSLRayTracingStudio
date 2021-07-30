#include "Common/Log.h"

int32 LogTrace::GetVarArgs(ANSICHAR* dest, int32 destSize, const ANSICHAR*& fmt, va_list argPtr)
{
	int32 result = vsnprintf(dest, destSize, fmt, argPtr);
	va_end(argPtr);
	return (result != -1 && result < (int32)destSize) ? result : -1;
}
