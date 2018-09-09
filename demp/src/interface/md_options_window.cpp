#include "md_options_window.h"

#include <SDL_image.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "../utility/md_util.h"
#include "../settings/music_player_settings.h"
#include "../app/realtime_system_application.h"



namespace mdEngine
{
	Window::OptionsWindow::OptionsWindow() :	m_Width(MP::Data::_OPTIONS_WINDOW_SIZE.x),
												m_Height(MP::Data::_OPTIONS_WINDOW_SIZE.y), 
												m_Window(NULL)
	{ }

	Window::OptionsWindow::~OptionsWindow()
	{
		SDL_DestroyRenderer(m_Renderer);
		SDL_DestroyWindow(m_Window);
		m_Window = NULL;
	}

	void Window::OptionsWindow::Init()
	{
		if (m_Window != NULL)
			return;

		m_Window = SDL_CreateWindow("Options",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			m_Width, 
			m_Height,
			SDL_WINDOW_SHOWN);

		m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
		m_WindowID = SDL_GetWindowID(m_Window);

		s32 imgFlags = IMG_INIT_PNG;
		if ((IMG_Init(imgFlags) & imgFlags) == false)
		{
			MD_SDL_ERROR("SDL_Image");
		}


		s32 xPos = 70;
		m_VolumeStepSlider = Interface::ButtonSlider(L"Volume Scroll Step:", glm::vec2(xPos, 20), &MP::Data::VolumeScrollStep, 1, 0, MP::Data::VolumeScrollStepMAX);
		m_VolumeStepSlider.Init(m_Renderer);

		m_PlaylistScrollStepSlider = Interface::ButtonSlider(L"Playlist Scroll Step:", glm::vec2(xPos, 70), &MP::Data::PlaylistScrollStep, 5.f, 0.f, MP::Data::PlaylistScrollStepMAX);
		m_PlaylistScrollStepSlider.Init(m_Renderer);

		m_PauseFadeTimeSlider = Interface::ButtonSlider(L"Volume Fade on Pause/Play Length:", glm::vec2(xPos, 120), &MP::Data::PauseFadeTime, 50, 0, MP::Data::PauseFadeTimeMAX);
		m_PauseFadeTimeSlider.Init(m_Renderer);

		m_RamLoadedSizeSlider = Interface::ButtonSlider(L"Maximum Ram Loaded Size:", glm::vec2(xPos, 170), &MP::Data::_MAX_SIZE_RAM_LOADED, 1, 0, MP::Data::_MAX_SIZE_RAM_LOADED_MAX);
		m_RamLoadedSizeSlider.Init(m_Renderer);

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
		}
		else
		{
			m_VolumeStepSlider.ResetButtons();
			m_PlaylistScrollStepSlider.ResetButtons();
			m_PauseFadeTimeSlider.ResetButtons();
			m_RamLoadedSizeSlider.ResetButtons();
		}

		m_VolumeStepSlider.Update();
		m_PlaylistScrollStepSlider.Update();
		m_PauseFadeTimeSlider.Update();
		m_RamLoadedSizeSlider.Update();
	}

	void Window::OptionsWindow::Render()
	{
		if (m_Window == NULL)
			return;

		SDL_SetRenderDrawColor(m_Renderer, SDLColor::Azure.r, SDLColor::Azure.g, SDLColor::Azure.b, 0xFF);
		SDL_RenderClear(m_Renderer);

		m_VolumeStepSlider.Render();
		m_PlaylistScrollStepSlider.Render();
		m_PauseFadeTimeSlider.Render();
		m_RamLoadedSizeSlider.Render();

		SDL_RenderPresent(m_Renderer);
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


	void Window::OptionsWindow::Free()
	{
		SDL_DestroyRenderer(m_Renderer);
		SDL_DestroyWindow(m_Window);
		m_VolumeStepSlider.Free();
		m_PlaylistScrollStepSlider.Free();
		m_PauseFadeTimeSlider.Free();
		m_RamLoadedSizeSlider.Free();
		IMG_Quit();
		m_Renderer = NULL;
		m_Window = NULL;
	}
}