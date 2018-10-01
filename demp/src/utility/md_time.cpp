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


		void Timer::Reset()
		{
			finished = false;
		}

		f32 Timer::GetProgress()
		{
			f32 progress = currentTime / targetTime;
			if (progress > 1.f)
				progress = 1.f;
			return progress;
		}


		f32 Timer::GetProgressLog()
		{
			if ((currentTime / targetTime) <= 0.1f)
				return 0;

			f32 progress = log10((currentTime / targetTime) * 10.f);
			if (progress > 1.f)
				progress = 1.f;
			return progress;
		}

		void Timer::Start()
		{
			stopped = false;
			if (stopped == false)
			{
				startTime = SDL_GetTicks();
				started = true;
				finished = false;
			}
		}

		void Timer::Stop()
		{
			stopped = true;
		}

		void Timer::Update()
		{
			if (started == true)
			{
				if (SDL_GetTicks() - startTime > targetTime &&
					currentTime - targetTime > deltaTime)
				{
					finished = true;
					started = false;
				}

				currentTime = SDL_GetTicks() - startTime;
			}
		}

		b8 Timer::IsFinished()
		{
				if (SDL_GetTicks() - startTime > targetTime &&
					currentTime - targetTime > deltaTime)
				{
					finished = true;
					started = false;

					return true;
				}
				currentTime = SDL_GetTicks() - startTime;

			return false;
		}

		f32 Timer::GetTicks()
		{
			return SDL_GetTicks() - startTime;
		}

		f32 Timer::GetTicksStart()
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