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

#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_sdl.h"
#include "../external/imgui/imgui_impl_opengl3.h">

#include "music_player_system.h"
#include "music_player_ui.h"
#include "md_time.h"

// TODO: create RealtimeApplication class that initializes all(sdl inits, input, main game loop) 
	//and MyApplicationHandler that will be passed to the construct of RealtimeApplication

namespace mdEngine
{
	SDL_Window* mdWindow = NULL;
	SDL_DisplayMode current;
	SDL_GLContext gl_context;
	const char* glsl_version = "#version 130";

	b8 mdIsRunning(false);
	b8 mdHasApplication(false);
	b8 mdIsActiveWindow(false);

	MP::ApplicationHandlerInterface* mdApplicationHandler(nullptr);

	extern u16 mdActualWindowWidth;
	extern u16 mdActualWindowHeight;
}

void mdEngine::OpenRealtimeApplication(mdEngine::MP::ApplicationHandlerInterface& applicationHandler)
{
	mdHasApplication = true;
	mdApplicationHandler = &applicationHandler;
	mdActualWindowWidth = 680;
	mdActualWindowHeight = 720;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		MD_SDL_ERROR("ERROR: SDL error");
		assert(SDL_Init(SDL_INIT_VIDEO) < 0);
		return;
	}

	if (BASS_Init(-1, 44100, 0, 0, NULL) == false)
	{
		MD_BASS_ERROR("ERROR: Initialize BASS");
		assert(SDL_Init(SDL_INIT_VIDEO) == false);
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);


	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GetCurrentDisplayMode(0, &current);
	mdWindow = SDL_CreateWindow("demp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mdActualWindowWidth, mdActualWindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (mdWindow == NULL)
	{
		MD_SDL_ERROR("ERROR: Failed to open SDL window");
		assert(mdWindow == NULL);

		return;
	}
	gl_context = SDL_GL_CreateContext(mdWindow);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	gl3wInit();


	/* create window */

	mdApplicationHandler->OnWindowOpen();

}

void mdEngine::RunRealtimeApplication(mdEngine::MP::ApplicationHandlerInterface& applicationHandler)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	ImGui_ImplSDL2_InitForOpenGL(mdWindow, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();

	mdIsRunning = true;

	SDL_Event event;

	Time::deltaTime = Time::time();
	
	f64 previousFrame = 0;
	f64 currentFrame = 0;

	while (mdIsRunning == true)
	{
		/* main loop */
		mdEngine::StartNewFrame();

		/* imgui */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(mdWindow);
		ImGui::NewFrame();

		
		
		/* Calcualte delta time */
		currentFrame = Time::time();
		Time::deltaTime = currentFrame - previousFrame;
		previousFrame = currentFrame;


		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			switch (event.type)
			{
			case (SDL_QUIT):
				mdIsRunning = false;
				break;
			case (SDL_DROPFILE):
			{
				std::wstring path = utf8_to_utf16(event.drop.file);
				MP::PushToPlaylist(path);
				//SDL_free(SDL_GetClipboardText());
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
		
		const u8* current_keystate = SDL_GetKeyboardState(NULL);
		mdEngine::UpdateKeyState(current_keystate);

		if (mdIsRunning == true)
		{
			mdApplicationHandler->OnRealtimeUpdate();

			SDL_UpdateWindowSurface(mdWindow);
		}


		ImGui::Render();
		SDL_GL_MakeCurrent(mdWindow, gl_context);
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(MP::UI::ClearColor.x, MP::UI::ClearColor.y, MP::UI::ClearColor.z, MP::UI::ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(mdWindow);
	}

}

void mdEngine::StopRealtimeApplication(mdEngine::MP::ApplicationHandlerInterface& applicationHandler)
{
	mdIsRunning = false;
}

void mdEngine::CloseRealtimeApplication(mdEngine::MP::ApplicationHandlerInterface& applicationHandler)
{
	/* CLEAR AND DELETE EVERYTHING */

	mdApplicationHandler->OnWindowClose();


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(mdWindow);

	BASS_Free();
	SDL_Quit();

	mdIsRunning = false;
	mdHasApplication = false;
	mdApplicationHandler = nullptr;
}

void mdEngine::SetWindowTitle(const char& windowTitle)
{
	/* set window title sdl */
}