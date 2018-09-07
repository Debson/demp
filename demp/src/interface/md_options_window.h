#ifndef MD_OPTIONS_WINDOW
#define MD_OPTIONS_WINDOW

#include "SDL.h"
#include "../utility/md_types.h"
#include "md_interface.h"

namespace mdEngine
{
	namespace Window
	{
		class OptionsWindow
		{
		public:
			OptionsWindow();
			~OptionsWindow();

			void Init();
			void Update();
			void Render();
			void ProcessEvents(SDL_Event* const e);
			b8 IsActive();



		private:
			void OnDestroy();

			f32 m_TestValue;

			s32 m_Height, m_Width;
			s32 m_WindowID;
			SDL_Window* m_Window;
			SDL_Renderer* m_Renderer;
			SDL_Event m_Event;
			SDL_GLContext m_Context;
			Interface::ButtonSlider m_VolumeStepSlider;


		};
	}
}

#endif // !MD_OPTIONS_WINDOW
