#pragma once

#define PROJECT_VERSION_MAJOR 0
#define PROJECT_VERSION_MINOR 0
#define PROJECT_VERSION_PATCH 0

#if defined(_WIN64)
    #define PLATFORM_64BITS 1
#else
    #define PLATFORM_64BITS	0
#endif

typedef unsigned char 		uint8;
typedef unsigned short int	uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

typedef	signed char			int8;
typedef signed short int	int16;
typedef signed int	 		int32;
typedef signed long long	int64;

typedef char				ANSICHAR;
typedef wchar_t				WIDECHAR;
typedef uint8				CHAR8;
typedef uint16				CHAR16;
typedef uint32				CHAR32;
typedef int32				TYPE_OF_NULL;
typedef decltype(nullptr)	TYPE_OF_NULLPTR;

#ifdef PLATFORM_WINDOWS
    #define FORCEINLINE     __inline
    #define CONSTEXPR       constexpr
#else
    #define FORCEINLINE     inline
    #define CONSTEXPR       constexpr
#endif
