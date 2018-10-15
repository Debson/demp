#include "md_helper_windows.h"

#include <SDL_syswm.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "../audio/mp_audio.h"
#include "../settings/music_player_settings.h"
#include "../graphics/graphics.h"
#include "../graphics/music_player_graphics_playlist.h"
#include "../settings/music_player_string.h"
#include "../app/realtime_system_application.h"
#include "../player/music_player_state.h"
#include "../utility/md_util.h"
#include "../utility/md_converter.h"


using namespace mdEngine::Graphics;

namespace mdEngine
{
	namespace Window
	{
		OptionsWindow*		mdOptionsWindow;
		LoadInfoWindow*		mdLoadInfoWindow;
		MusicInfoWindow*	mdMusicInfoWindow;

		std::map<std::string, WindowObject*> WindowsContainer;

		void UpdateWindowContainer();
	}


	void Window::UpdateWindows()
	{
		if (Window::mdOptionsWindow != nullptr)
		{
			if (Window::mdOptionsWindow->IsActive() == false)
			{
				Window::WindowsContainer.erase("OptionsWindow");
				delete Window::mdOptionsWindow;
				Window::mdOptionsWindow = nullptr;
				State::ResetState(State::OptionWindow::HasFocus);
			}
		}

		if (Window::mdMusicInfoWindow != nullptr)
		{
			if (Window::mdMusicInfoWindow->IsActive() == false)
			{
				Window::WindowsContainer.erase("MusicInfoWindow");
				delete Window::mdMusicInfoWindow;
				Window::mdMusicInfoWindow = nullptr;
			}
		}

	}

	void Window::RenderWindows()
	{

	}

	void Window::UpdateWindowContainer()
	{
		for (auto & i : WindowsContainer)
		{
			if (i.second == nullptr)
				md_log(i.first);
		}
	}

	void Window::WindowObject::ProcessEvents(SDL_Event* const e)
	{
		if (m_Window == NULL)
			return;

		m_WindowMoved = false;

		if (e->type == SDL_WINDOWEVENT && e->window.windowID == m_WindowID)
		{
			switch (e->window.event)
			{
			case SDL_WINDOWEVENT_CLOSE:
				OnDelete();
				break;
			case SDL_WINDOWEVENT_ENTER:
				m_WindowHasFocus = true;
				break;
			case SDL_WINDOWEVENT_LEAVE:
				m_WindowHasFocus = false;
				break;
			case SDL_WINDOWEVENT_MOVED:
				m_WindowMoved = true;
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				m_HasFocus = false;
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				m_HasFocus = true;
				break;
			}
		}
	}

	s32 Window::WindowObject::GetWindowID() const
	{
		return m_WindowID;
	}

	b8 Window::WindowObject::IsActive()
	{
		return m_Active;
	}

	Window::OptionsWindow::OptionsWindow()
	{ 
		m_Width = MP::Data::_OPTIONS_WINDOW_SIZE.x;
		m_Height = MP::Data::_OPTIONS_WINDOW_SIZE.y;
		
		m_Window = NULL;

		m_Window = SDL_CreateWindow(Strings::_OPTIONS_WINDOW_NAME.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			m_Width,
			m_Height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_SKIP_TASKBAR);

		assert(m_Window != NULL);

#ifdef _WIN32_
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(m_Window, &info);

		// Disable minimize/maximize buttons
		SetWindowLong(info.info.win.window, GWL_STYLE,
			GetWindowLong(info.info.win.window, GWL_STYLE) & ~WS_MINIMIZEBOX);
#else

#endif

		m_Active = true;


		m_WindowID = SDL_GetWindowID(m_Window);

		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());

		m_Projection = glm::ortho(0.f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.f);
		m_Shader = Shader::shaderDefault;
		m_Shader->use();
		m_Shader->setMat4("projection", m_Projection);


		glViewport(0, 0, static_cast<GLint>(m_Width), static_cast<GLint>(m_Height));

		//SDL_GL_MakeCurrent(m_Window, 0);
		s32 xPos = 40;
		m_VolumeStepSlider = Interface::ButtonSlider(Strings::_VOLUME_SCROL_STEP_TEXT, glm::vec2(xPos, 20), &MP::Data::VolumeScrollStep, 1, 0, MP::Data::VolumeScrollStepMAX);
		m_VolumeStepSlider.Init(m_Shader);

		m_PlaylistScrollStepSlider = Interface::ButtonSlider(Strings::_PLAYLIST_SCROLL_STEP_TEXT, glm::vec2(xPos, 70), &MP::Data::PlaylistScrollStep, 5.f, 0.f, MP::Data::PlaylistScrollStepMAX);
		m_PlaylistScrollStepSlider.Init(m_Shader);

		m_PauseFadeTimeSlider = Interface::ButtonSlider(Strings::_PAUSE_FADE_TEXT, glm::vec2(xPos, 120), &MP::Data::PauseFadeTime, 50, 0, MP::Data::PauseFadeTimeMAX);
		m_PauseFadeTimeSlider.Init(m_Shader);

		m_RamLoadedSizeSlider = Interface::ButtonSlider(Strings::_MAX_RAM_LOADED_SIZE_TEXT, glm::vec2(xPos, 170), &MP::Data::_MAX_SIZE_RAM_LOADED, 1, 0, MP::Data::_MAX_SIZE_RAM_LOADED_MAX);
		m_RamLoadedSizeSlider.Init(m_Shader);

		m_ToTrayOnExitState = State::CheckState(State::OnExitMinimizeToTray);
		m_ToTrayOnExit = Interface::CheckBox(Strings::_ON_EXIT_MINMIZE_TO_TRAY_TEXT, glm::vec2(xPos, 220), &m_ToTrayOnExitState);
		m_ToTrayOnExit.Init(m_Shader);

		m_OnLoadCheckExistence = Interface::CheckBox(Strings::_ON_LOAD_CHECK_PATH_EXISTENCE, glm::vec2(xPos, 270), &MP::Settings::IsPathExistenceCheckingEnabled);
		m_OnLoadCheckExistence.Init(m_Shader);


	}

	Window::OptionsWindow::~OptionsWindow()
	{

		m_Active = false;
		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());

		m_VolumeStepSlider.Free();
		m_PlaylistScrollStepSlider.Free();;
		m_PauseFadeTimeSlider.Free();
		m_RamLoadedSizeSlider.Free();

		m_ToTrayOnExit.Free();
		m_OnLoadCheckExistence.Free();

		SDL_DestroyWindow(m_Window);
		m_Shader = NULL;
		m_Window = NULL;

	}

	void Window::OptionsWindow::Update()
	{
		if (m_Window == NULL)
			return;

		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());

		// Reset to default values
		if (m_VolumeStepSlider.IsDefaultPressed() == true)
		{
			MP::Data::VolumeScrollStep = MP::Data::Default::VolumeScrollStep;
			m_VolumeStepSlider.ReloadSliderInfo();
		}

		if (m_PlaylistScrollStepSlider.IsDefaultPressed() == true)
		{
			MP::Data::PlaylistScrollStep = MP::Data::Default::PlaylistScrollStep;
			m_PlaylistScrollStepSlider.ReloadSliderInfo();
		}

		if (m_PauseFadeTimeSlider.IsDefaultPressed() == true)
		{
			MP::Data::PauseFadeTime = MP::Data::Default::PauseFadeTime;
			m_PauseFadeTimeSlider.ReloadSliderInfo();
		}

		if (m_RamLoadedSizeSlider.IsDefaultPressed() == true)
		{
			MP::Data::_MAX_SIZE_RAM_LOADED = MP::Data::Default::_MAX_SIZE_RAM_LOADED;
			m_RamLoadedSizeSlider.ReloadSliderInfo();
		}

		// Check if mouse is inside options window
		if (m_WindowHasFocus == true)
		{
			m_VolumeStepSlider.ProcessInput();
			m_PlaylistScrollStepSlider.ProcessInput();
			m_PauseFadeTimeSlider.ProcessInput();
			m_RamLoadedSizeSlider.ProcessInput();

			m_ToTrayOnExit.ProcessInput();
			m_OnLoadCheckExistence.ProcessInput();
		}
		else
		{
			m_VolumeStepSlider.ResetButtons();
			m_PlaylistScrollStepSlider.ResetButtons();
			m_PauseFadeTimeSlider.ResetButtons();
			m_RamLoadedSizeSlider.ResetButtons();

			m_ToTrayOnExit.ResetState();
			m_OnLoadCheckExistence.ResetState();
		}

		m_ToTrayOnExitState == true ? State::SetState(State::OnExitMinimizeToTray) : State::ResetState(State::OnExitMinimizeToTray);

		m_VolumeStepSlider.Update();
		m_PlaylistScrollStepSlider.Update();
		m_PauseFadeTimeSlider.Update();
		m_RamLoadedSizeSlider.Update();
		
		m_ToTrayOnExit.Update();
		m_OnLoadCheckExistence.Update();
	}

	void Window::OptionsWindow::Render()
	{
		if (m_Window == NULL)
			return;

		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());
		glClearColor(Color::Azure.r, Color::Azure.g, Color::Azure.b, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_Shader->setMat4("projection", m_Projection);
		glViewport(0, 0, static_cast<GLint>(m_Width), static_cast<GLint>(m_Height));


		m_VolumeStepSlider.Render();
		m_PlaylistScrollStepSlider.Render();
		m_PauseFadeTimeSlider.Render();
		m_RamLoadedSizeSlider.Render();

		m_ToTrayOnExit.Render();
		m_OnLoadCheckExistence.Render();

		SDL_GL_SwapWindow(m_Window);
	}

	void Window::OptionsWindow::OnDelete()
	{
		m_Active = false;
	}

	Window::LoadInfoWindow::LoadInfoWindow() { }

	Window::LoadInfoWindow::LoadInfoWindow(glm::vec2 size, glm::vec4 playerWindowDim)
	{
		m_Width = size.x;
		m_Height = size.y;
		m_Window = NULL;
		CancelWasPressed = false;


		m_Window = SDL_CreateWindow(Strings::_OPTIONS_WINDOW_NAME.c_str(),
			playerWindowDim.x + (m_Width - playerWindowDim.z) / 2.f,
			playerWindowDim.y + playerWindowDim.w / 2.f,
			m_Width,
			m_Height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_SKIP_TASKBAR);

		assert(m_Window != NULL);
		
		m_WindowID = SDL_GetWindowID(m_Window);


		m_Active = true;
#ifdef _WIN32_
		// Set this window to be always on top
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(m_Window, &info);

		SetWindowPos(info.info.win.window, HWND_TOPMOST,
			playerWindowDim.x - (m_Width - playerWindowDim.z) / 2.f,
			playerWindowDim.y + playerWindowDim.w / 2.f,
			m_Width, m_Height,
			SWP_NOSIZE);


		SetWindowLong(info.info.win.window, GWL_STYLE,
			GetWindowLong(info.info.win.window, GWL_STYLE) & ~WS_MINIMIZEBOX);
#else

#endif
		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());

		m_Projection = glm::ortho(0.f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.f);
		m_Shader = Shader::shaderDefault;
		m_Shader->use();
		m_Shader->setMat4("projection", m_Projection);

		glViewport(0, 0, static_cast<GLint>(m_Width), static_cast<GLint>(m_Height));

		s32 widthOffset = 20;
		m_ProgressBarSize = glm::vec2(m_Width - 20.f, 15.f);
		m_ProgressBarPos = glm::vec2((m_Width - m_ProgressBarSize.x) / 2.f, m_Height / 2.f - m_ProgressBarSize.y);

		s32 loadingPathTextOffsetY = 20;
		m_LoadingPathText = Text::TextObject(Color::Black);
		m_LoadingPathText.SetTextPos(glm::vec2(m_ProgressBarPos.x, m_ProgressBarPos.y - loadingPathTextOffsetY));


		m_CancelButtonSize = glm::vec2(m_ProgressBarSize.x / 6.f, 20.f);
		m_CancelButtonPos = glm::vec2(m_ProgressBarPos.x + (m_ProgressBarSize.x - m_CancelButtonSize.x) / 2.f,
			m_ProgressBarPos.y + m_ProgressBarSize.y + 20.f);
		m_CancelButton = new Interface::Button(m_CancelButtonSize, m_CancelButtonPos);

		m_CancelText = Text::TextObject(Color::Black);
		m_CancelText.SetTextString("Cancel");
		m_CancelText.InitTextTexture();

		m_CancelText.SetTextPos(glm::vec2(100, 50));
		m_CancelText.SetTextPos(glm::vec2(m_CancelButtonPos.x + (m_CancelButtonSize.x - m_CancelText.GetTextSize().x) / 2.f,
			m_CancelButtonPos.y + (m_CancelButtonSize.y - m_CancelText.GetTextSize().y) / 2.f));

		CancelWasPressed = false;

		//Window::WindowsContainer.insert(std::pair< std::string, std::shared_ptr<Window::WindowObject>>("LoadInfoWindow", this));
	}
	
	Window::LoadInfoWindow::~LoadInfoWindow()
	{
		m_Active = false;
		Window::WindowsContainer.erase("LoadInfoWindow");

		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());

		m_LoadingPathText.DeleteTexture();
		m_CancelText.DeleteTexture();
		delete m_CancelButton;

		m_WindowID = -1;


		SDL_DestroyWindow(m_Window);
		//State::SetState(State::UpdatePlaylistInfoStrings);


		m_Shader = NULL;
		m_Window = NULL;
	}

	void Window::LoadInfoWindow::Update()
	{
		if (m_Window == NULL || m_HasFocus == false)
			return;

		App::ProcessButton(m_CancelButton);

		if (m_CancelButton->isPressed == true)
		{
			md_log("pressed");
			CancelWasPressed = true;
			m_Active = false;
		}

		u32 test = Audio::GetIndexOfLoadingObject();
		auto audioCon = Audio::Object::GetAudioObjectContainer();
		if (Audio::Object::GetAudioObject(Audio::GetIndexOfLoadingObject() - 1) != nullptr)
		{
			std::string test = Audio::Object::GetAudioObject(Audio::GetIndexOfLoadingObject() - 1)->GetPath();
			m_LoadingPathText.SetTextString(Audio::Object::GetAudioObject(Audio::GetIndexOfLoadingObject() - 1)->GetPath());
			m_LoadingPathText.ReloadTextTexture();
		}

		m_BarProgress = (float)Audio::Object::GetSize() / (float)Audio::GetFilesAddedCount();
		std::string title = "[" + std::to_string(Audio::Object::GetSize()) + "/";
		title += std::to_string(Audio::GetFilesAddedCount()) + "]";
		title += "Playlist name - Adding files...";
		SDL_SetWindowTitle(m_Window, title.c_str());
	}

	void Window::LoadInfoWindow::Render()
	{
		if (m_Window == NULL)
			return;

		// TODO: Bug(GL_INVALID_VALUE)
		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());

		glClearColor(Color::Azure.r, Color::Azure.g, Color::Azure.b, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_Shader->use();
		m_Shader->setMat4("projection", m_Projection);
		glViewport(0, 0, static_cast<GLint>(m_Width), static_cast<GLint>(m_Height));

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(m_ProgressBarPos, 1.0));
		model = glm::scale(model, glm::vec3(m_ProgressBarSize.x * m_BarProgress, m_ProgressBarSize.y, 1.0));
		m_Shader->setMat4("model", model);
		m_Shader->setBool("plain", true);
		m_Shader->setVec3("color", Color::Orange);
		Shader::Draw(m_Shader);
		m_Shader->setVec3("color", Color::White);
		m_Shader->setBool("plain", false);

		Shader::DrawOutline(glm::vec4(m_ProgressBarPos, m_ProgressBarSize), 0.95f, Color::Azure);
		Shader::DrawOutline(glm::vec4(m_ProgressBarPos, m_ProgressBarSize), 1.1f);


		// Cancel button
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(m_CancelButtonPos, 0.91f));
		model = glm::scale(model, glm::vec3(m_CancelButtonSize, 1.0));
		m_Shader->setMat4("model", model);
		m_Shader->setBool("plain", true);
		m_Shader->setVec3("color", Color::Grey);
		Shader::Draw(m_Shader);
		m_Shader->setVec3("color", Color::White);
		m_Shader->setBool("plain", false);

		Shader::DrawOutline(glm::vec4(m_CancelButtonPos, m_CancelButtonSize), 1.1f);

		m_LoadingPathText.DrawString();
		m_CancelText.DrawString(true);

		SDL_GL_SwapWindow(m_Window);
	}

	void Window::LoadInfoWindow::OnDelete()
	{
		this->CancelWasPressed = true;
	}

	Window::MusicInfoWindow::MusicInfoWindow(glm::vec2 pos)
	{
		m_Width = 400;
		m_Height = 150;

		m_Window = NULL;

		m_Window = SDL_CreateWindow(Strings::_OPTIONS_WINDOW_NAME.c_str(),
			pos.x,
			pos.y,
			m_Width,
			m_Height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
			SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_BORDERLESS);

		assert(m_Window != NULL);


#ifdef _WIN32_
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(m_Window, &info);

		// always on top
		SetWindowPos(info.info.win.window, HWND_TOPMOST,
			pos.x, pos.y,
			m_Width, m_Height,
			SWP_NOSIZE);

		// Disable minimize/maximize buttons
		SetWindowLong(info.info.win.window, GWL_STYLE,
			GetWindowLong(info.info.win.window, GWL_STYLE) & ~WS_MINIMIZEBOX);
#else

#endif

		m_Active = true;


		m_WindowID = SDL_GetWindowID(m_Window);

		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());

		m_Projection = glm::ortho(0.f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.f);
		m_Shader = Shader::shaderDefault;
		m_Shader->use();
		m_Shader->setMat4("projection", m_Projection);

		glViewport(0, 0, static_cast<GLint>(m_Width), static_cast<GLint>(m_Height));

		m_Movable = Interface::Movable(glm::vec2(m_Width, m_Height), glm::vec2(0.f), false);
		f32 exitButtonSize = 15.f;
		m_ExitButton = Interface::Button(glm::vec2(exitButtonSize), glm::vec2(m_Width - exitButtonSize, 0.f));
		m_ButtonCont.push_back(&m_ExitButton);

		s32 oX = 10;
		s32 oY = 20;
		m_AlbumPicSize = glm::vec2(150.f, 100.f);
		m_AlbumPicPos = glm::vec2(m_Width - m_AlbumPicSize.x - oX, oY);
		s32 textPosX = 4.f;

		// title text should be in larger font
		s32 playingID = Graphics::MP::GetPlaylistObject()->GetPlayingID();
		m_TitleText = Text::TextObject(Color::Red, 18);
		std::string titleStr;
		if (Audio::Object::GetAudioObject(playingID)->GetTitle().compare("") != 0)
		{
			m_TitleText.SetTextString(Audio::Object::GetAudioObject(playingID)->GetTitle());
		}
		else
		{
			titleStr = Audio::Object::GetAudioObject(playingID)->GetCompleteName();
			m_TitleText.SetTextString(Converter::GetShortenString(titleStr, m_AlbumPicPos.x + textPosX, 18));
		}

		m_TitleText.SetTextPos(glm::vec2(textPosX, 10.f));
		m_TitleText.InitTextTexture();

		s32 offsetY = 3.f;
		m_AlbumText = Text::TextObject(Color::White);
		m_AlbumText.SetTextString(Audio::Object::GetAudioObject(playingID)->GetAlbum());
		m_AlbumText.InitTextTexture();
		m_AlbumText.SetTextPos(glm::vec2(textPosX, m_TitleText.GetTextSize().y + m_TitleText.GetTextPos().y + offsetY));


		m_ArtistText = Text::TextObject(Color::White);
		m_ArtistText.SetTextString(Audio::Object::GetAudioObject(playingID)->GetArtist());
		m_ArtistText.SetTextPos(glm::vec2(textPosX, m_AlbumText.GetTextSize().y + m_AlbumText.GetTextPos().y));
		m_ArtistText.InitTextTexture();

		// space

		// time text should be in larger font
		offsetY = 25.f;
		m_TimeText = Text::TextObject(Color::Red, 18);
		m_TimeText.SetTextString(Converter::SecToProperTimeFormatShort(Audio::Object::GetAudioObject(playingID)->GetLength()));
		m_TimeText.SetTextPos(glm::vec2(textPosX, m_ArtistText.GetTextSize().y + m_ArtistText.GetTextPos().y + offsetY));
		m_TimeText.InitTextTexture();


		m_InfoText = Text::TextObject(Color::White);
		std::string inf = std::to_string((s32)Audio::Object::GetAudioObject(playingID)->GetFrequency() / 1000) + " kHz, ";
		inf + std::to_string((s32)Audio::Object::GetAudioObject(playingID)->GetBitrate()) + " kbps, ";
		inf += Converter::BytesToProperSizeFormat(Audio::Object::GetAudioObject(playingID)->GetObjectSize());


		m_InfoText.SetTextString(inf);
		m_InfoText.SetTextPos(glm::vec2(textPosX, m_TimeText.GetTextSize().y + m_TimeText.GetTextPos().y));
		m_InfoText.InitTextTexture();


		m_PathText = Text::TextObject(Color::White);
		s32 slashPos = Audio::Object::GetAudioObject(playingID)->GetPath().find_last_of('\\');
		inf = Audio::Object::GetAudioObject(playingID)->GetPath().substr(slashPos + 1, Audio::Object::GetAudioObject(playingID)->GetPath().length());
		Converter::GetShortenString(inf, m_Width);
		m_PathText.SetTextString(inf);
		m_PathText.SetTextPos(glm::vec2(textPosX, m_InfoText.GetTextSize().y + m_InfoText.GetTextPos().y));
		m_PathText.InitTextTexture();



		m_FocusLostTimer = Time::Timer(2500);
	}

	Window::MusicInfoWindow::~MusicInfoWindow()
	{
		m_Active = false;

		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());

		m_WindowID = -1;

		SDL_DestroyWindow(m_Window);

		m_Shader = NULL;
		m_Window = NULL;
	}

	void Window::MusicInfoWindow::Update()
	{
		if (m_Window == NULL)
			return;

		App::ProcessButton(&m_ExitButton);

		if (m_ExitButton.isPressed == true)
		{
			OnDelete();
		}

		if (m_WindowMoved == true)
		{
			m_Movable.m_Pos = Window::GetWindowPos(m_Window);
		}

		if(m_HasFocus == true)
			App::ProcessMovable(&m_Movable, m_Window, &m_ButtonCont);

		if (m_HasFocus == false && m_FocusLostTimer.started == false && m_FocusLostTimer.finished == false)
		{
			m_FocusLostTimer.Start();
		}

		if (m_FocusLostTimer.finished == true)
		{
			OnDelete();
		}

		if (m_HasFocus == true && m_FocusLostTimer.started == true)
		{
			m_FocusLostTimer.Stop();
			m_FocusLostTimer.Reset();
		}

		m_FocusLostTimer.Update();
	}

	void Window::MusicInfoWindow::Render()
	{
		if (m_Window == NULL)
			return;

		SDL_GL_MakeCurrent(m_Window, *Window::GetMainWindowContext());

		glClearColor(Color::DarkGrey.r, Color::DarkGrey.g, Color::DarkGrey.b, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_Shader->use();
		m_Shader->setMat4("projection", m_Projection);
		glViewport(0, 0, static_cast<GLint>(m_Width), static_cast<GLint>(m_Height));

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(m_ExitButton.GetButtonPos(), 1.f));
		model = glm::scale(model, glm::vec3(m_ExitButton.GetButtonSize(), 1.f));
		m_Shader->setBool("plain", true);
		m_Shader->setVec3("color", Color::Green);
		m_Shader->setMat4("model", model);
		Shader::Draw(m_Shader);
		m_Shader->setVec3("color", Color::White);
		m_Shader->setBool("plain", false);

		m_TitleText.DrawString();
		m_AlbumText.DrawString();
		m_ArtistText.DrawString();

		m_TimeText.DrawString();
		m_InfoText.DrawString();
		m_PathText.DrawString();

		if (Audio::Object::GetAudioObject(Graphics::MP::GetPlaylistObject()->GetPlayingID()) != nullptr)
		{
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(m_AlbumPicPos, 1.f));
			model = glm::scale(model, glm::vec3(m_AlbumPicSize, 1.f));
			m_Shader->setMat4("model", model);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Audio::Object::GetAudioObject(Graphics::MP::GetPlaylistObject()->GetPlayingID())->GetAlbumPictureTexture());
			Shader::Draw(m_Shader);
		}



		SDL_GL_SwapWindow(m_Window);
	}

	void Window::MusicInfoWindow::OnDelete()
	{
		m_Active = false;
	}

}