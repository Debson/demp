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
		const glm::vec3 White(1.f);
		const glm::vec3 Black(0.f);
		const glm::vec3 Grey(0.85f);
		const glm::vec3 DarkGrey(0.5f);
		const glm::vec3 Red(1.f, 0.f, 0.f);
		const glm::vec3 Green(0.f, 1.f, 0.f);
		const glm::vec3 Blue(0.f, 0.f, 1.f);
		const glm::vec3 Pink(1.f, 0.0784f, 0.576f);
		const glm::vec3 Azure(240.f / 255.f, 1.f, 1.f);
		const glm::vec3 Silver(192.f / 255.f);
	}

	namespace SDLColor
	{
		const SDL_Color White = { 255, 255, 255 };
		const SDL_Color Black = { 0, 0, 0 };
		const SDL_Color Grey = { 216, 216, 216 };
		const SDL_Color DarkGrey = { 127, 127, 127 };
		const SDL_Color Red = { 255, 0, 0 };
		const SDL_Color Green = { 0, 255, 0 };
		const SDL_Color Blue = { 0, 0, 255 };
		const SDL_Color Orange = { 255, 140, 0 };
		const SDL_Color Azure = { 240, 255, 255 };
		const SDL_Color Silver = { 192, 192, 192 };
	}
}
#endif // !MD_TYPES_H
