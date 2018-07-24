#pragma once
#ifndef MD_TIME_H
#define MD_TIME_H


#include <SDL.h>
#include "md_types.h"

namespace mdEngine 
{
namespace Time
{
	extern f64 deltaTime;

	f64 time();

	u64 getTicks();

	void sleep(f32 time);
}
}
#endif // !MD_TIME_H