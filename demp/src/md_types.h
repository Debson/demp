#pragma once
#ifndef MD_TYPES_H
#define MD_TYPES_H

#include <vector>
#include <stdint.h>

#include "configuration.h"

namespace mdEngine
{
	typedef bool b8;

	typedef int8_t		s8;
	typedef int16_t		s16;
	typedef int32_t		s32;
	typedef int64_t		s64;

	typedef uint8_t		u8;
	typedef uint16_t	u16;
	typedef uint32_t	u32;
	typedef uint64_t	u64;

	typedef float		f32;
	typedef double		f64;

	typedef long unsigned int uint_64;

	struct AppTime
	{
		f32 total;
		f32 delta;
	};


#ifdef _WIN32_
	typedef std::vector<std::wstring> PathContainer;
#else
	typedef std::vector<const char*> PathContainer;
#endif
}

#endif // !MD_TYPES_H
