#ifndef MD_OPTIONS_WINDOW
#define MD_OPTIONS_WINDOW

#include <map>

#include "SDL.h"
#include <SDL_syswm.h>

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
			b8 m_HasFocus;
			b8 m_ToTrayOnExitState;
			b8 m_WindowHasFocus;
			b8 m_WindowMoved;
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
			Time::Timer m_ColorTimer;

			Text::TextObject m_LoadingPathText;
			Text::TextObject m_CancelText;
			glm::vec3		 m_Color;

			glm::vec2 m_ProgressBarPos;
			glm::vec2 m_ProgressBarSize;

			Interface::Button* m_CancelButton;
			glm::vec2 m_CancelButtonPos;
			glm::vec2 m_CancelButtonSize;
		};

		class MusicInfoWindow : public WindowObject
		{
		public:
			MusicInfoWindow(glm::vec2 pos, s32 id = -1);
			~MusicInfoWindow();


			void Update();
			void Render();
			void OnDelete();

			void DeleteAlbumPicOnClose();
			s32 GetViewedMusicInfoID();

		private:

			b8 m_DeleteAlbumPic;
			s32 m_MusicID;
			GLuint m_AlbumPicTex;

			glm::vec2 m_AlbumPicSize;
			glm::vec2 m_AlbumPicPos;

			Time::Timer m_FocusLostTimer;
			Interface::Movable m_Movable;
			Interface::Button m_ExitButton;

			SDL_SysWMinfo m_Info;

			Text::TextObject m_TitleText;
			Text::TextObject m_AlbumText;
			Text::TextObject m_ArtistText;
			Text::TextObject m_YearText;

			Text::TextObject m_TimeText;
			Text::TextObject m_InfoText;
			Text::TextObject m_PathText;


			std::vector<Interface::Button*> m_ButtonCont;

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
