#include "realtime_system_application.h"

#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <codecvt>

#include <SDL.h>
#undef main // SDL_main.h is included automatically from SDL.h, so you always get the nasty #define.
#include <SDL_mixer.h>

#include <bass.h>
#include <GL/gl3w.h>

#ifdef _DEBUG_
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_sdl.h"
#include "../external/imgui/imgui_impl_opengl3.h">
#endif // DEBUG

#include "music_player_system.h"
#include "music_player_ui.h"
#include "md_time.h"
#include "application_window.h"
#include "graphics.h"

// TODO: create RealtimeApplication class that initializes all(sdl inits, input, main game loop) 
	//and MyApplicationHandler that will be passed to the construct of RealtimeApplication

namespace mdEngine
{
	SDL_Window* mdWindow = NULL;
	SDL_Window* mdWindowOpenGL = NULL;
	SDL_DisplayMode current;
	SDL_GLContext gl_context;
#ifdef _DEBUG_
	ImGuiIO io;
	const char* glsl_version = "#version 130";
#endif

	b8 mdIsRunning(false);
	b8 mdHasApplication(false);
	b8 mdIsActiveWindow(false);

	App::ApplicationHandlerInterface* mdApplicationHandler(nullptr);

	s32 mdActualWindowWidth;
	s32 mdActualWindowHeight;
	s32 mdCurrentWindowWidth;
	s32 mdCurrentWindowHeight;
	float clean_color = 0.5f;

	void SetupSDL();

	void SetupOpenGL();

	void SetupImGui();
}

void mdEngine::SetupSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		MD_SDL_ERROR("ERROR: SDL error");
		assert(SDL_Init(SDL_INIT_VIDEO) < 0);
		return;
	}

	mdWindow = SDL_CreateWindow("demp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		mdActualWindowWidth, mdActualWindowHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);

	mdWindowOpenGL = SDL_CreateWindow("demp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		mdActualWindowWidth, mdActualWindowHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);

	if (mdWindow == NULL)
	{
		MD_SDL_ERROR("ERROR: Failed to open SDL window");
		assert(mdWindow == NULL);

		return;
	}


	SDL_SetWindowModalFor(mdWindowOpenGL, mdWindow);

	SDL_SetWindowOpacity(mdWindow, 1.f);
	SDL_SetWindowOpacity(mdWindowOpenGL, 0.5f);
}

void mdEngine::SetupOpenGL()
{
	gl_context = SDL_GL_CreateContext(mdWindow);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GetCurrentDisplayMode(0, &current);


}

void mdEngine::SetupImGui()
{

	gl_context = SDL_GL_CreateContext(mdWindow);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	gl3wInit();

	glViewport(0, 0, 500, 800);

#ifdef _DEBUG_
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	io = ImGui::GetIO(); (void)io;
	ImGui_ImplSDL2_InitForOpenGL(mdWindow, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();
#endif
}

void mdEngine::OpenRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	mdHasApplication = true;
	mdApplicationHandler = &applicationHandler;
	App::WindowProperties windowProperties;
	//App::WinProperties = windowProperties;
	mdApplicationHandler->CollectWindowProperties(windowProperties);
	mdActualWindowWidth = mdCurrentWindowWidth = windowProperties.mWindowWidth;
	mdActualWindowHeight = mdCurrentWindowHeight = windowProperties.mWindowHeight;


	if (BASS_Init(-1, 44100, 0, 0, NULL) == false)
	{
		MD_BASS_ERROR("ERROR: Initialize BASS");
		assert(SDL_Init(SDL_INIT_VIDEO) == false);
		return;
	}

	SetupSDL();

	SetupOpenGL();

	SetupImGui();

	/* create window */

	mdApplicationHandler->OnWindowOpen();
	Graphics::Start();

}

void mdEngine::RunRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	mdIsRunning = true;

	SDL_Event event;

	Time::deltaTime = Time::time();
	
	f64 previousFrame = 0;
	f64 currentFrame = 0;

	while (mdIsRunning == true)
	{
		/* main loop */
		mdEngine::StartNewFrame();
#ifdef _DEBUG_
		/* imgui */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(mdWindow);
		ImGui::NewFrame();
#endif

		/* Calcualte delta time */
		currentFrame = Time::time();
		Time::deltaTime = currentFrame - previousFrame;
		previousFrame = currentFrame;


		if (SDL_PollEvent(&event))
		{
#ifdef _DEBUG_
			ImGui_ImplSDL2_ProcessEvent(&event);
#endif
			switch (event.type)
			{
			case (SDL_QUIT):
				mdIsRunning = false;
				break;
			case (SDL_DROPFILE):
			{
#ifdef _WIN32_
				std::wstring path = utf8_to_utf16(event.drop.file);
				MP::PushToPlaylist(path);
#else
				MP::PushToPlaylist(event.drop.file);
#endif
				SDL_free(SDL_GetClipboardText());
				break;
			}
			case (SDL_MOUSEWHEEL):
				UpdateScrollPosition(event.wheel.x, event.wheel.y);
				break;
			case (SDL_MOUSEMOTION):
				UpdateMousePosition(event.motion.x, event.motion.y);
				break;
			}
		}

		GetWindowSize(&mdCurrentWindowWidth, &mdCurrentWindowHeight);
		
		const u8* current_keystate = SDL_GetKeyboardState(NULL);
		mdEngine::UpdateKeyState(current_keystate);

		const u32 current_mousestate = SDL_GetMouseState(NULL, NULL);
		mdEngine::UpdateMouseState(current_mousestate);


		if (mdIsRunning == true)
		{
			/* graphics render */

			mdApplicationHandler->OnRealtimeUpdate();
			Graphics::Update();
		}

		SDL_GL_MakeCurrent(mdWindow, gl_context);
#ifdef _DEBUG_
		glClearColor(MP::UI::ClearColor.x, MP::UI::ClearColor.y, MP::UI::ClearColor.z, MP::UI::ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, mdCurrentWindowWidth, mdCurrentWindowHeight);
		Graphics::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
		glViewport(0, 0, (int)mdActualWindowWidth, (int)mdActualWindowHeight);
		glClearColor(clean_color, clean_color, clean_color, clean_color);
		glClear(GL_COLOR_BUFFER_BIT);
#endif
		SDL_GL_SwapWindow(mdWindow);
	}

}

void mdEngine::StopRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	mdIsRunning = false;
}

void mdEngine::CloseRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	/* CLEAR AND DELETE EVERYTHING */


	mdApplicationHandler->OnWindowClose();
	Graphics::Close();

#ifdef _DEBUG_
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
#endif

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(mdWindow);

	BASS_Free();
	SDL_Quit();

	mdIsRunning = false;
	mdHasApplication = false;
	mdApplicationHandler = nullptr;
}

void mdEngine::AppExit()
{
	mdIsRunning = false;
}

void mdEngine::SetWindowProperties(const App::WindowProperties& windowProperties)
{
	mdActualWindowWidth = windowProperties.mWindowWidth;
	mdActualWindowHeight = windowProperties.mWindowHeight;
}

void mdEngine::SetWindowTitle(const b8& windowTitle)
{
	/* set window title sdl */
}

void mdEngine::SetWindowPos(s32 x, s32 y)
{
	SDL_SetWindowPosition(mdWindow, x, y);
}

void mdEngine::GetWindowSize(s32* w, s32* h)
{
	SDL_GetWindowSize(mdWindow, w, h);
}

void mdEngine::GetWindowScale(f32* scaleX, f32* scaleY)
{
	//std::cout << mdCurrentWindowWidth << "   " << mdActualWindowWidth << std::endl;

	*scaleX = (float)mdCurrentWindowWidth / (float)mdActualWindowWidth;
	*scaleY = (float)mdCurrentWindowHeight / (float)mdActualWindowHeight;
}

void mdEngine::GetWindowPos(s32* x, s32* y)
{
	SDL_GetWindowPosition(mdWindow, x, y);
}