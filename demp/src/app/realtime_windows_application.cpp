#include "realtime_system_application.h"

#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <Windows.h>

#include <thread>

#include <SDL.h>
#undef main // SDL_main.h is included automatically from SDL.h, so you always get the nasty #define.
#include <SDL_ttf.h>
#include <SDL_syswm.h>

#include <bass.h>
#include <GL/gl3w.h>

#ifdef _DEBUG_
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_sdl.h"
#include "../../external/imgui/imgui_impl_opengl3.h">
#endif // DEBUG

#include "../app/application_window.h"
#include "../app/realtime_application.h"
#include "../player/music_player_system.h"
#include "../player/music_player_state.h"
#include "../ui/music_player_ui.h"
#include "../audio/mp_audio.h"
#include "../audio/mp_channel_attrib.h"
#include "../graphics/graphics.h"
#include "../sqlite/md_sqlite.h";
#include "../utility/md_time.h"


namespace mdEngine
{
	SDL_Window* mdWindow = NULL;
	SDL_DisplayMode current;
	SDL_GLContext gl_context;
	SDL_SysWMinfo wmInfo;
	HWND hwnd;
	std::thread fileLoadThread;
	std::thread graphicsThread, updateThread;

#ifdef _DEBUG_
	ImGuiIO io;
	const char* glsl_version = "#version 130";
#endif

	b8 mdIsRunning(false);
	b8 mdAppClosing(false);
	b8 mdHasApplication(false);
	b8 mdIsActiveWindow(false);

	App::ApplicationHandlerInterface* mdApplicationHandler(nullptr);
	App::WindowProperties Window::windowProperties;

	s32 mdActualWindowWidth;
	s32 mdActualWindowHeight;

	/* Used when window is resizable */
	s32 mdCurrentWindowWidth;
	s32 mdCurrentWindowHeight;

	float clean_color = 1.0f;

	void SetupSDL();

	void SetupOpenGL();

	void SetupImGui();

	void UpdateWindowSize();

	void ProceedToSafeClose();
}

void mdEngine::SetupSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		MD_SDL_ERROR("ERROR: SDL error");
		assert(SDL_Init(SDL_INIT_VIDEO) < 0);
		return;
	}

	SDL_GetCurrentDisplayMode(0, &current);

	mdWindow = SDL_CreateWindow("demp", Window::windowProperties.mWindowPositionX, Window::windowProperties.mWindowPositionY,
		Window::windowProperties.mWindowWidth, current.h,
		SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);


	if (mdWindow == NULL)
	{
		MD_SDL_ERROR("ERROR: Failed to open SDL window");
		assert(mdWindow == NULL);

		return;
	}

	if (TTF_Init() == -1)
	{
		std::cout << "ERROR: TTF not initialized!\n";
		return;
	}

	/* Retrieve hwnd window info and set transparecny for specific color.
	   Works only on Windows. 
	   TODO: implement that on linux later on.
	*/
#ifdef _WIN32_
	SDL_GetWindowWMInfo(mdWindow, &wmInfo);
	hwnd = wmInfo.info.win.window;
	SetWindowLong(hwnd, GWL_EXSTYLE,
		GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);


	SetLayeredWindowAttributes(hwnd, RGB(0xFF, 0xFE, 0xFF), 0, LWA_COLORKEY);

#endif

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
	
}

void mdEngine::SetupImGui()
{

	SDL_GL_SetSwapInterval(0); // Enable vsync
	
	gl3wInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);

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

void mdEngine::UpdateWindowSize()
{
	if (State::CheckState(State::Window::Resized) == true)
	{
		SDL_GetWindowSize(mdWindow, &Window::windowProperties.mWindowWidth, &Window::windowProperties.mWindowHeight);
	}
}

void mdEngine::ProceedToSafeClose()
{
	Audio::Info::WaitTillFileInfoLoaded();

	mdIsRunning = false;
}

void mdEngine::OpenRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	mdHasApplication = true;
	mdApplicationHandler = &applicationHandler;
	mdApplicationHandler->CollectWindowProperties(Window::windowProperties);


	if (BASS_Init(-1, 48000, 0, 0, NULL) == false)
	{
		MD_BASS_ERROR("ERROR: Initialize BASS");
		assert(SDL_Init(SDL_INIT_VIDEO) == false);
		return;
	}

	SetupSDL();

	/* Get max display height and update window's size structure */
	Window::windowProperties.mWindowHeight = current.h;
	mdActualWindowWidth = mdCurrentWindowWidth = Window::windowProperties.mWindowWidth;
	mdActualWindowHeight = mdCurrentWindowHeight = Window::windowProperties.mWindowHeight;
	

	SetupOpenGL();

	SetupImGui();

	/* create window */

	mdApplicationHandler->OnWindowOpen();
	Graphics::StartGraphics();

	SDL_CaptureMouse(SDL_TRUE);

	glViewport(0, 0, mdCurrentWindowWidth, mdCurrentWindowHeight);
}

void mdEngine::RunRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	mdIsRunning = true;
	mdAppClosing = false;

	SDL_Event event;

	Time::deltaTime = Time::Time();
	
	f64 previousFrame = 0;
	f64 currentFrame = 0;
	Time::Timer capTimer;

	while (mdIsRunning == true)
	{
		capTimer.start();
		/* main loop */
		mdEngine::StartNewFrame();
#ifdef _DEBUG_
		/* imgui */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(mdWindow);
		ImGui::NewFrame();
#endif

		/* Calcualte delta time */
		currentFrame = Time::Time();
		Time::deltaTime = currentFrame - previousFrame;
		previousFrame = currentFrame;

		auto optionsWindow = MP::UI::GetOptionsWindow();

		if (SDL_PollEvent(&event))
		{
#ifdef _DEBUG_
			ImGui_ImplSDL2_ProcessEvent(&event);
#endif

			optionsWindow->ProcessEvents(&event);

			switch (event.type)
			{
			case (SDL_QUIT):
				ProceedToSafeClose();
				break;
			case (SDL_DROPFILE):
			{
#ifdef _WIN32_
				State::SetState(State::FileDropped);
				State::ResetState(State::PathLoadedFromFileVolatile);
				std::wstring p(utf8_to_utf16(event.drop.file));
				Audio::PushToPlaylist(p);
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

			if (event.type == SDL_WINDOWEVENT)
			{
				switch (event.window.event)
				{
				case (SDL_WINDOWEVENT_FOCUS_GAINED):
					State::SetState(State::Window::HasFocus);
					break;
				case (SDL_WINDOWEVENT_FOCUS_LOST):
					State::ResetState(State::Window::HasFocus);;
					break;
				case (SDL_WINDOWEVENT_ENTER):
					State::SetState(State::Window::MouseEnter);
					break;
				case (SDL_WINDOWEVENT_LEAVE):
					State::ResetState(State::Window::MouseEnter);;
					break;
				case(SDL_WINDOWEVENT_MINIMIZED):
					State::SetState(State::Window::Minimized);
					break;
				}
			}
		}
		
		const u8* current_keystate = SDL_GetKeyboardState(NULL);
		mdEngine::UpdateKeyState(current_keystate);

		const u32 current_mousestate = SDL_GetMouseState(NULL, NULL);
		mdEngine::UpdateMouseState(current_mousestate);

		UpdateRelativeMousePosition();

		//if (mdIsRunning == true && Window::windowProperties.mMouseWindowEvent == App::WindowEvent::kEnter)
		{
			UpdateWindowSize();
			mdApplicationHandler->OnRealtimeUpdate();
			Graphics::UpdateGraphics();
		}

		SDL_GL_MakeCurrent(mdWindow, gl_context);
#ifdef _DEBUG_
		glClearColor(MP::UI::ClearColor.x, MP::UI::ClearColor.y, MP::UI::ClearColor.z, MP::UI::ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (State::CheckState(State::Window::Minimized) == false)
		{
			Graphics::RenderGraphics();
			mdApplicationHandler->OnRealtimeRender();
		}


		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
		glClearColor(1.f, 254.f/255.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, (int)mdActualWindowWidth, (int)mdActualWindowHeight);
		Graphics::Render();
#endif
		SDL_GL_SwapWindow(mdWindow);

	
		f32 frameTicks = capTimer.getTicks();
		if (frameTicks < App::Data::_SCREEN_TICK_PER_FRAME)
		{
			SDL_Delay(App::Data::_SCREEN_TICK_PER_FRAME - frameTicks);
		}
	}
}

void mdEngine::StopRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	mdIsRunning = false;
}

void mdEngine::CloseRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	/* CLEAR AND FREE MEMORY */


	mdApplicationHandler->OnWindowClose();
	Graphics::CloseGraphics();

#ifdef _DEBUG_
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
#endif

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(mdWindow);

	TTF_Quit();
	BASS_Free();
	SDL_Quit();

	mdIsRunning = false;
	mdHasApplication = false;
	mdApplicationHandler = nullptr;
}

void mdEngine::AppExit()
{
	mdAppClosing = true;

	Audio::Info::WaitTillFileInfoLoaded();

	mdIsRunning = false;
}

b8 mdEngine::IsAppClosing()
{
	return mdAppClosing;
}

void mdEngine::SetWindowProperties(const App::WindowProperties& windowProperties)
{
	mdActualWindowWidth = windowProperties.mWindowWidth;
	mdActualWindowHeight = windowProperties.mWindowHeight;
}

SDL_Window* mdEngine::Window::GetSDLWindow()
{
	return mdWindow;
}

HWND mdEngine::Window::GetHWNDWindow()
{
	return hwnd;
}

void mdEngine::Window::SetWindowTitle(const b8& windowTitle)
{
	/* set window title sdl */
}

void mdEngine::Window::SetWindowPos(s32 x, s32 y)
{
	SDL_SetWindowPosition(mdWindow, x, y);
}

void mdEngine::Window::SetWindowSize(s32 w, s32 h)
{
	SDL_SetWindowSize(mdWindow, w, h);
}

void mdEngine::Window::GetWindowSize(s32* w, s32* h)
{
	SDL_GetWindowSize(mdWindow, w, h);
}

void mdEngine::Window::GetWindowScale(f32* scaleX, f32* scaleY)
{
	*scaleX = (float)mdCurrentWindowWidth / (float)mdActualWindowWidth;
	*scaleY = (float)mdCurrentWindowHeight / (float)mdActualWindowHeight;
}

void mdEngine::Window::GetWindowPos(s32* x, s32* y)
{
	SDL_GetWindowPosition(mdWindow, x, y);
}

