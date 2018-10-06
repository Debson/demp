#include "md_options_window.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "../utility/md_util.h"
#include "../settings/music_player_settings.h"
#include "../graphics/graphics.h"
#include "../settings/music_player_string.h"
#include "../app/realtime_system_application.h"
#include "../player/music_player_state.h"

using namespace mdEngine::Graphics;

namespace mdEngine
{
	Window::OptionsWindow::OptionsWindow() :	m_Width(MP::Data::_OPTIONS_WINDOW_SIZE.x),
												m_Height(MP::Data::_OPTIONS_WINDOW_SIZE.y), 
												m_Window(NULL)
	{ }

	Window::OptionsWindow::~OptionsWindow()
	{
		SDL_DestroyWindow(m_Window);
		m_Window = NULL;
	}

	void Window::OptionsWindow::Init()
	{
		if (m_Window != NULL)
			return;

		m_Window = SDL_CreateWindow(Strings::_OPTIONS_WINDOW_NAME.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			m_Width, 
			m_Height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);


		m_WindowID = SDL_GetWindowID(m_Window);

		m_Projection = glm::ortho(0.f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.f);
		m_Shader = Shader::shaderDefault;
		m_Shader->use();
		m_Shader->setMat4("projection", m_Projection);

		glViewport(0, 0, static_cast<GLint>(m_Width), static_cast<GLint>(m_Height));

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

	void Window::OptionsWindow::Update()
	{
		if (m_Window == NULL)
			return;

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

	void Window::OptionsWindow::ProcessEvents(SDL_Event* const e)
	{
		if (m_Window == NULL)
			return;

		if (e->type == SDL_WINDOWEVENT && e->window.windowID == m_WindowID)
		{
			switch (e->window.event)
			{
			case SDL_WINDOWEVENT_CLOSE:
				Free();
				break;
			case SDL_WINDOWEVENT_ENTER:
				m_WindowHasFocus = true;
				break;
			case SDL_WINDOWEVENT_LEAVE:
				m_WindowHasFocus = false;
				break;
			}
		}
	}

	b8 Window::OptionsWindow::IsActive()
	{
		return m_Window != NULL;
	}

	s32 Window::OptionsWindow::GetOptionWindowID()
	{
		return m_WindowID;
	}

	void Window::OptionsWindow::Free()
	{
		SDL_DestroyWindow(m_Window);

		m_VolumeStepSlider.Free();
		m_PlaylistScrollStepSlider.Free();;
		m_PauseFadeTimeSlider.Free();
		m_RamLoadedSizeSlider.Free();

		m_ToTrayOnExit.Free();
		m_OnLoadCheckExistence.Free();

		m_Shader = NULL;
		m_Window = NULL;
	}
}