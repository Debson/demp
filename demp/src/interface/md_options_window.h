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
			void Free();
			b8 IsActive();

		private:

			b8 m_WindowHasFocus;
			s32 m_Height, m_Width;
			s32 m_WindowID;
			SDL_Window* m_Window;
			SDL_Renderer* m_Renderer;
			SDL_Event m_Event;

			Interface::ButtonSlider m_VolumeStepSlider;
			Interface::ButtonSlider m_PlaylistScrollStepSlider;
			Interface::ButtonSlider m_PauseFadeTimeSlider;
			Interface::ButtonSlider m_RamLoadedSizeSlider;


		};
	}
}

#endif // !MD_OPTIONS_WINDOW
