#include "md_time.h"

namespace mdEngine
{
	namespace Time
	{
		Timer::Timer() 
		{ 
			finished = false;
			started = false;
		}

		Timer::Timer(f32 time) : targetTime(time) 
		{
			finished = false;
			started = false;
		}

		Timer::~Timer() { }


		void Timer::reset()
		{
			finished = false;
		}

		f32 Timer::progress()
		{
			return currentTime / targetTime;
		}

		void Timer::start()
		{
			startTime = getTicks();
			started = true;
			finished = false;
		}

		void Timer::update()
		{
			if (started == true)
			{
				if (getTicks() - startTime > targetTime)
				{
					finished = true;
					started = false;
				}

				currentTime = getTicks() - startTime;
			}
		}
	

		f64 deltaTime = 0;


		f64 Time::time()
		{
			return (SDL_GetTicks() / 1000.0);
		}

		u64 Time::getTicks()
		{
			return SDL_GetTicks();
		}

		void Time::sleep(f32 time)
		{
			SDL_Delay(u32(time * 1000.f));
		}

	}
}