#ifndef MD_OPTIONS_WINDOW
#define MD_OPTIONS_WINDOW

#include "SDL.h"
#include "../utility/md_types.h"
#include "md_interface.h"
#include "../utility/md_shape.h"

namespace mdEngine
{
	namespace Window
	{
		class WindowObject
		{
		public:

			virtual ~WindowObject() { };

			virtual void Init();
			virtual void Update() = 0;
			virtual void Render() = 0;
			virtual void ProcessEvents(SDL_Event* const e);
			virtual void Free() = 0;

			b8 IsActive();

		protected:

			b8 m_ToTrayOnExitState;
			b8 m_WindowHasFocus;
			s32 m_Height, m_Width;
			s32 m_WindowID;
			mdShader* m_Shader;
			glm::mat4 m_Projection;
			SDL_Event m_Event;
			SDL_Window* m_Window;
			Text::TextObject m_TestText;
		};

		class OptionsWindow : public WindowObject
		{
		public:
			OptionsWindow();
			~OptionsWindow();

			void Init();
			void Update();
			void Render();
			void Free();
			s32 GetOptionWindowID();

		private:


			Interface::ButtonSlider m_VolumeStepSlider;
			Interface::ButtonSlider m_PlaylistScrollStepSlider;
			Interface::ButtonSlider m_PauseFadeTimeSlider;
			Interface::ButtonSlider m_RamLoadedSizeSlider;
			Interface::CheckBox		m_ToTrayOnExit;
			Interface::CheckBox		m_OnLoadCheckExistence;
		};

		class LoadInfoWindow : public WindowObject
		{
		public:
			LoadInfoWindow();
			LoadInfoWindow(f32 width, f32 height);

			void Init(glm::vec4 playerWindowDim);
			void Update();
			void Render();
			void Free();
			s32 GetOptionWindowID();
			b8 CancelWasPressed;

		private:

			f32 m_BarProgress;
				

			Text::TextObject m_LoadingPathText;
			Text::TextObject m_CancelText;

			glm::vec2 m_ProgressBarPos;
			glm::vec2 m_ProgressBarSize;

			Interface::Button* m_CancelButton;
			glm::vec2 m_CancelButtonPos;
			glm::vec2 m_CancelButtonSize;
		};

	}
}

#endif // !MD_OPTIONS_WINDOW
