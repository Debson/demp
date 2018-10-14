#ifndef MD_OPTIONS_WINDOW
#define MD_OPTIONS_WINDOW

#include <map>

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

			virtual void Update() = 0;
			virtual void Render() = 0;
			virtual void ProcessEvents(SDL_Event* const e);
			virtual void OnDelete() = 0;
			s32 GetWindowID() const;
			b8 IsActive();

		protected:

			b8 m_Active;
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

			void Update();
			void Render();
			void OnDelete();

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
			LoadInfoWindow(glm::vec2 size, glm::vec4 playerWindowDim);
			~LoadInfoWindow();

			void Update();
			void Render();
			void OnDelete();
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

		class MusicInfoWindow : public WindowObject
		{
		public:
			MusicInfoWindow(glm::vec2 pos);
			~MusicInfoWindow();


			void Update();
			void Render();
			void OnDelete();

		private:

		};

		extern OptionsWindow*	mdOptionsWindow;
		extern LoadInfoWindow*	mdLoadInfoWindow;
		extern MusicInfoWindow* mdMusicInfoWindow;

		extern std::map<std::string, WindowObject*> WindowsContainer;


		void UpdateWindows();
		void RenderWindows();
	}
}

#endif // !MD_OPTIONS_WINDOW
