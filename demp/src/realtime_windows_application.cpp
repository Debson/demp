#include "realtime_system_application.h"
#include "system_music_player.h"
#include "md_time.h"
#include "assert.h"

#include <SDL.h>
#undef main // SDL_main.h is included automatically from SDL.h, so you always get the nasty #define.
#include <SDL_mixer.h>

#include <bass.h>


// TODO: create RealtimeApplication class that initializes all(sdl inits, input, main game loop) 
	//and MyApplicationHandler that will be passed to the construct of RealtimeApplication

namespace mdEngine
{
	SDL_Window* mdWindow;
	SDL_Surface* mdScreenSurface;

	b8 mdIsRunning(false);
	b8 mdHasApplication(false);
	b8 mdIsActiveWindow(false);

	Application::ApplicationHandlerInterface* mdApplicationHandler(nullptr);

	extern u16 mdActualWindowWidth;
	extern u16 mdActualWindowHeight;
}

void mdEngine::OpenRealtimeApplication(mdEngine::Application::ApplicationHandlerInterface& applicationHandler)
{
	mdHasApplication = true;
	mdApplicationHandler = &applicationHandler;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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

	mdActualWindowWidth = 300;
	mdActualWindowHeight = 500;

	mdWindow = SDL_CreateWindow("demp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mdActualWindowWidth, mdActualWindowHeight, SDL_WINDOW_SHOWN);

	if (mdWindow == NULL)
	{
		MD_SDL_ERROR("ERROR: Failed to open SDL window");
		assert(mdWindow == NULL);

		return;
	}
	/*
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 16384) < 0)
	{
		MD_SDL_ERROR("ERROR: Failed to initialize SDL_mixer");
		return;
	}
	*/
	mdScreenSurface = SDL_GetWindowSurface(mdWindow);

	/* create window */

	mdApplicationHandler->OnWindowOpen();

}

void mdEngine::RunRealtimeApplication(mdEngine::Application::ApplicationHandlerInterface& applicationHandler)
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
		
		/* Calcualte delta time */
		currentFrame = Time::time();
		Time::deltaTime = currentFrame - previousFrame;
		previousFrame = currentFrame;


		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case (SDL_QUIT):
				mdIsRunning = false;
				break;
			case (SDL_DROPFILE):
				MP::PushToPlaylist(event.drop.file);
				break;
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

	}

}

void mdEngine::StopRealtimeApplication(mdEngine::Application::ApplicationHandlerInterface& applicationHandler)
{
	mdIsRunning = false;
}

void mdEngine::CloseRealtimeApplication(mdEngine::Application::ApplicationHandlerInterface& applicationHandler)
{
	/* CLEAR AND DELETE EVERYTHING */

	mdApplicationHandler->OnWindowClose();

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