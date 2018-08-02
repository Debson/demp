#pragma once
#ifndef MD_TIME_H
#define MD_TIME_H


#include <SDL.h>
#include "md_types.h"

namespace mdEngine 
{
namespace Time
{
	struct Timer
	{
		Timer();
		Timer(f32 time);
		~Timer();

		f32 targetTime;
		f32 startTime;
		f32 currentTime;

		b8 started;
		b8 finished;

		f32 progress();
		void reset();
		void start();
		void update();
	};

	extern f64 deltaTime;

	f64 time();

	u64 getTicks();

	void sleep(f32 time);
}
}
#endif // !MD_TIME_H