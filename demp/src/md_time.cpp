#include "md_time.h"

namespace mdEngine
{
	namespace Time
	{
		Timer::Timer() 
		{ 
			finished = false;
			started = false;
			stopped = true;
			startTime = 0.f;
			targetTime = 0.f;
			currentTime = 0.f;
		}

		Timer::Timer(f32 time) : targetTime(time) 
		{
			finished = false;
			started = false;
			startTime = 0.f;
			currentTime = 0.f;
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
			stopped = false;
			if (stopped == false)
			{
				startTime = SDL_GetTicks();
				started = true;
				finished = false;
			}
		}

		void Timer::stop()
		{
			stopped = true;
		}

		void Timer::update()
		{
			if (started == true)
			{
				if (SDL_GetTicks() - startTime > targetTime)
				{
					finished = true;
					started = false;
				}

				currentTime = SDL_GetTicks() - startTime;
			}
		}

		f32 Timer::getTicks()
		{
			return SDL_GetTicks() - startTime;
		}

		f32 Timer::getTicksStart()
		{
			if (stopped == false)
			{
				return SDL_GetTicks() - startTime;
			}

			return 0;
		}
	

		f64 deltaTime = 0;


		f64 Time::Time()
		{
			return (SDL_GetTicks() / 1000.0);
		}

		u64 Time::GetTicks()
		{
			return SDL_GetTicks();
		}

		void Time::sleep(f32 time)
		{
			SDL_Delay(u32(time * 1000.f));
		}

	}
}