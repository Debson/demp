#pragma once
#ifndef MD_TYPES_H
#define MD_TYPES_H

#include <vector>
#include <stdint.h>
#include <glm.hpp>
#include <SDL.h>

#include "../settings/configuration.h"

#define POS_INVALID (-1)

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
	typedef std::vector<std::wstring*> PathContainer;
#else
	typedef std::vector<const char*> PathContainer;
#endif

	namespace Color
	{
		extern glm::vec3 White;
		extern glm::vec3 Black;
		extern glm::vec3 Grey;
		extern glm::vec3 DarkGrey;
		extern glm::vec3 Red;
		extern glm::vec3 Green;
		extern glm::vec3 Blue;
	}

	namespace SDLColor
	{
		extern SDL_Color White;
		extern SDL_Color Black;
		extern SDL_Color Grey;
		extern SDL_Color DarkGrey;
		extern SDL_Color Red;
		extern SDL_Color Green;
		extern SDL_Color Blue;
	}
}
#endif // !MD_TYPES_H
