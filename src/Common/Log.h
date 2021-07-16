#pragma once

#include <stdio.h>

#define _DEBUG_PREFIX_NAME(A) #A
#define DEBUG_PREFIX_NAME(A) _DEBUG_PREFIX_NAME(A)

#define DEBUG_LOG_PREFIX    [ DEBUG ]:
#define INFO_LOG_PREFIX     [ INFO \40]:
#define WARNING_LOG_PREFIX  [WARNING]:
#define ERROR_LOG_PREFIX    [ ERROR ]:
#define FAULT_LOG_PREFIX    [ FAULT ]:

#if defined(DEBUG) || defined(_DEBUG)
    #define LOGD(...)  do { fprintf(stdout, "%s", DEBUG_PREFIX_NAME(DEBUG_LOG_PREFIX)); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); } while (0)
#else
    #define LOGD(...)  do {  } while (0)
#endif

#define LOGI(...)  do { fprintf(stdout, "%s",    DEBUG_PREFIX_NAME(INFO_LOG_PREFIX)); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); } while (0)
#define LOGW(...)  do { fprintf(stdout, "%s", DEBUG_PREFIX_NAME(WARNING_LOG_PREFIX)); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); } while (0)
#define LOGE(...)  do { fprintf(stdout, "%s",   DEBUG_PREFIX_NAME(ERROR_LOG_PREFIX)); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); } while (0)
#define LOGF(...)  do { fprintf(stdout, "%s",   DEBUG_PREFIX_NAME(FAULT_LOG_PREFIX)); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); } while (0)
