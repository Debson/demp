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

		m_TestValue = 5;
		m_VolumeStepSlider = Interface::ButtonSlider(L"test:", glm::vec2(100, 100), &m_TestValue, 1.f);
		m_VolumeStepSlider.Init(m_Renderer);

	}

	void Window::OptionsWindow::Update()
	{
		if (m_Window == NULL)
			return;

		m_VolumeStepSlider.Update();
	}

	void Window::OptionsWindow::Render()
	{
		if (m_Window == NULL)
			return;

		SDL_SetRenderDrawColor(m_Renderer, 0xFF, 0x0, 0xFF, 0xFF);
		SDL_RenderClear(m_Renderer);

		m_VolumeStepSlider.Render();

		SDL_RenderPresent(m_Renderer);
	}

	void Window::OptionsWindow::ProcessEvents(SDL_Event* const e)
	{
		if (m_Window == NULL)
			return;

		if (e->type == SDL_WINDOWEVENT)
		{
			switch (e->window.event)
			{
			case SDL_WINDOWEVENT_CLOSE:
				OnDestroy();
				break;
			}
		}

	}


	void Window::OptionsWindow::OnDestroy()
	{
		SDL_DestroyRenderer(m_Renderer);
		SDL_DestroyWindow(m_Window);
		m_VolumeStepSlider.Free();
		IMG_Quit();
		m_Renderer = NULL;
		m_Window = NULL;
	}
}