#pragma once

#include "Common/Log.h"

#include <assert.h>

#define ASSERT_IMP(test, ...)   do { if (!test) { LOGE("%s(%d) : \n", __FILE__, __LINE__); LOGE(##__VA_ARGS__); } assert(test); } while (0)
#define Assert(test)			ASSERT_IMP((test), "Assertion failed on expression: '" #test "'")
#define AssertMsg(test, ...)	ASSERT_IMP((test), ##__VA_ARGS__)