#include "md_options_window.h"

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
		SDL_GL_DeleteContext(m_Context);
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
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

		/*m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
		m_WindowID = SDL_GetWindowID(m_Window);*/

		m_Context = SDL_GL_CreateContext(m_Window);

		assert(m_Context != NULL);

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		glViewport(0, 0, m_Width, m_Height);


		m_TestValue = 5.f;
		m_VolumeStepSlider = Interface::ButtonSlider(glm::vec2(100, 100), &m_TestValue, 1.f);

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

		glm::vec3 clearColor = Color::Red;
		SDL_GL_MakeCurrent(m_Window, m_Context);
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glViewport(0, 0, m_Width, m_Height);

		/*SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
		SDL_RenderClear(m_Renderer);*/

		m_VolumeStepSlider.Render();

		//SDL_RenderPresent(m_Renderer);
		SDL_GL_SwapWindow(m_Window);
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
		SDL_GL_DeleteContext(m_Context);
		SDL_DestroyWindow(m_Window);
		m_Window = NULL;
	}
}