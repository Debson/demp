#include "md_time.h"

namespace mdEngine
{
	namespace Time
	{
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