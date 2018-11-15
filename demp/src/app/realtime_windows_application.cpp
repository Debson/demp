#include "realtime_system_application.h"

#include <string>
#include <Windows.h>
#include <shellapi.h>
#include <cstdlib>

#include <thread>

#include <SDL.h>
#undef main // SDL_main.h is included automatically from SDL.h, so you always get the nasty #define.
#include <SDL_ttf.h>
#include <SDL_syswm.h>

#include <bass.h>
#include <GL/gl3w.h>
#include <boost/filesystem.hpp>



#ifdef _DEBUG_
#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_sdl.h"
#include "../../external/imgui/imgui_impl_opengl3.h">
#endif // DEBUG

#include "../app/application_window.h"
#include "../app/realtime_application.h"
#include "../player/music_player_system.h"
#include "../player/music_player_state.h"
#include "../player/music_player_config.h"
#include "../ui/music_player_ui.h"
#include "../audio/mp_audio.h"
#include "../audio/mp_channel_attrib.h"
#include "../graphics/graphics.h"
#include "../graphics/music_player_graphics.h"
#include "../sqlite/md_sqlite.h";
#include "../utility/md_time.h"

#define ICO1 101
#define IDM_EXIT 32771
#define ID_S_EXIT                       32772
#define IDM_SHOW_DEMP					32773
#define IDM_SEP 32774

// Windows Header Files:
#include <windows.h>
#include <shellapi.h>
// C RunTime Header Files
#include <tchar.h>



namespace fs = boost::filesystem;


namespace mdEngine
{
	SDL_Window* mdWindow;
	SDL_DisplayMode current;
	SDL_GLContext gl_context;
	SDL_SysWMinfo wmInfo;
	HWND hwnd;
	NOTIFYICONDATA icon;
	BOOL bDisable = FALSE;
	HMENU hPopMenu;

	s32 mdWindowID;
	std::thread* fileLoadThread;	
	std::vector<std::thread*> threadCon;
	


#ifdef _DEBUG_
	ImGuiIO io;
	const char* glsl_version = "#version 130";
#endif

	b8 mdIsRunning(false);
	b8 mdAppClosing(false);
	b8 mdHasApplication(false);
	b8 mdIsActiveWindow(false);

	App::ApplicationHandlerInterface* mdApplicationHandler(nullptr);
	App::WindowProperties Window::WindowProperties;
	App::MonitorProperties Window::MonitorProperties;
	App::TrayIconProperties Window::TrayIconProperties;

	s32 mdActualWindowWidth;
	s32 mdActualWindowHeight;

	/* Used when window is resizable */
	s32 mdCurrentWindowWidth;
	s32 mdCurrentWindowHeight;

	float clean_color = 1.0f;

	void SetupSDL();

	void SetupOpenGL();

	void SetupGlew();

#ifdef _WIN32_
	void SetupWindows();
#else

#endif

	void UpdateWindowSize();

	void ProceedToSafeClose();

	b8 CheckIfInstanceExists();
}

b8 mdEngine::CheckIfInstanceExists()
{
	auto m_singleInstanceMutex = CreateMutex(NULL, TRUE, "demp_md");
	if (m_singleInstanceMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND existingApp = FindWindow(0, "demp");
		if (existingApp) SetForegroundWindow(existingApp);

		int argc = 0;
		LPWSTR * strCmdLine = CommandLineToArgvW(GetCommandLineW(), &argc);
		if (argc == 2)
		{
			std::fstream file;
			file.open(Strings::_TEMP_CHILD_CONSOLE_ARG, std::ios::out | std::ios::binary);
			if (file.is_open() == false)
			{
				MD_ERROR("Error: Could not open a file for writing!\n");
				return false;
			}

			file << utf16_to_utf8(strCmdLine[1]);
			file.close();
		}

		LocalFree(strCmdLine);

		return TRUE;
	}

	return FALSE;
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

	mdWindow = SDL_CreateWindow("demp", Window::WindowProperties.m_WindowPositionX, Window::WindowProperties.m_WindowPositionY,
		MP::Data::_DEFAULT_PLAYER_SIZE.x + MP::Data::_WINDOW_TEXTBOXES_OFFSET, current.h,
		SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);

	mdWindowID = SDL_GetWindowID(mdWindow);
	
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
}

void mdEngine::SetupOpenGL()
{
	gl_context = SDL_GL_CreateContext(mdWindow);

	SDL_GL_MakeCurrent(mdWindow, gl_context);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	/*SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);*/
	
}

void mdEngine::SetupGlew()
{
	SDL_GL_SetSwapInterval(0); // disable vsync
	
	gl3wInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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

#ifdef _WIN32_
void mdEngine::SetupWindows()
{
	/* Retrieve hwnd window info and set transparecny for specific color.
   Works only on Windows.
   TODO: implement that on linux later on.
*/
	SDL_GetWindowWMInfo(mdWindow, &wmInfo);
	hwnd = wmInfo.info.win.window;
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	//SetLayeredWindowAttributes(hwnd, RGB(127, 127, 127), 0, LWA_COLORKEY);

	SetLayeredWindowAttributes(hwnd, RGB(0xFF, 0xFE, 0xFF), 0, ULW_COLORKEY);


	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

	SDL_GetWindowWMInfo(mdWindow, &wmInfo);
	SDL_VERSION(&wmInfo.version);

	if (SDL_GetWindowWMInfo(mdWindow, &wmInfo))
	{
		ZeroMemory(&icon, sizeof(NOTIFYICONDATA));
		icon.cbSize = sizeof(NOTIFYICONDATA);
		icon.hWnd = wmInfo.info.win.window;
		icon.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		icon.uCallbackMessage = WM_USER + 1;
		icon.hIcon = (HICON)LoadImage(NULL, TEXT("demp.ico"), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		strcpy_s(icon.szTip, "demp");
		Window::TrayIconProperties.m_TraySize = glm::ivec2(32);
		bool success = Shell_NotifyIcon(NIM_ADD, &icon);
	}

}
#else

#endif

void mdEngine::UpdateWindowSize()
{
	/*if (State::CheckState(State::Window::Resized) == true)
	{
		SDL_GetWindowSize(mdWindow, &Window::windowProperties.mWindowWidth, &Window::windowProperties.mWindowHeight);
	}*/
}

void mdEngine::ProceedToSafeClose()
{
	mdIsRunning = false;
}

void mdEngine::OpenRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	// Initialize config path before anything is done
	Strings::InitializeStrings();

	// App is already opened, exit it
	if (CheckIfInstanceExists() == true)
	{
		exit(1);
	}

	mdHasApplication = true;
	mdApplicationHandler = &applicationHandler;
	mdApplicationHandler->CollectWindowProperties(Window::WindowProperties);


	if (BASS_Init(-1, 48000, 0, 0, NULL) == false)
	{
		MD_BASS_ERROR("ERROR: Initialize BASS");
		assert(SDL_Init(SDL_INIT_VIDEO) == false);
		return;
	}

	SetupSDL();

	/* Get max display height and update window's size structure */
	Window::WindowProperties.m_WindowHeight = current.h;
	//Window::windowProperties.mWindowWidth = current.w;
	mdActualWindowWidth = mdCurrentWindowWidth = Window::WindowProperties.m_WindowWidth;
	mdActualWindowHeight = mdCurrentWindowHeight = Window::WindowProperties.m_WindowHeight;

	RECT rect;
	HWND taskBar = FindWindowW(L"Shell_traywnd", NULL);
	GetWindowRect(taskBar, &rect);
	Window::MonitorProperties.m_MonitorWidth	= current.w;
	Window::MonitorProperties.m_MonitorHeight	= current.h;
	Window::MonitorProperties.m_TaskBarHeight	= rect.bottom - rect.top;


	SetupOpenGL();
	SetupGlew();

	mdIsRunning = true;
	mdAppClosing = false;

	
	mdApplicationHandler->OnWindowOpen();
	Graphics::StartGraphics();

	SetupWindows();

	SDL_CaptureMouse(SDL_TRUE);

	glViewport(0, 0, mdCurrentWindowWidth, mdCurrentWindowHeight);
}



void mdEngine::RunRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	if (State::CheckState(State::Window::Exit) == true)
		return;

	SDL_Event event;

	Time::deltaTime = Time::Time();

	f64 previousFrame = 0;
	f64 currentFrame = 0;
	Time::Timer capTimer;

	while (mdIsRunning == true)
	{
		capTimer.Start();
		/* main loop */
		mdEngine::StartNewFrame();
		State::StartNewFrame();


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

		if (State::CheckState(State::Window::MouseOnTrayIcon) == true)
		{
			glm::vec2 mousePos = App::Input::GetGlobalMousePosition();
			b8 inside = (mousePos.x < Window::TrayIconProperties.m_TrayPos.x + Window::TrayIconProperties.m_TraySize.x) &&
						 mousePos.x > Window::TrayIconProperties.m_TrayPos.x && 
						(mousePos.y > Window::TrayIconProperties.m_TrayPos.y) && (mousePos.y < Window::TrayIconProperties.m_TrayPos.y + Window::TrayIconProperties.m_TraySize.y);
			
			if(inside == false)
				State::ResetState(State::Window::MouseOnTrayIcon);
		}
		while (SDL_PollEvent(&event) != 0)
		{
#ifdef _DEBUG_
			ImGui_ImplSDL2_ProcessEvent(&event);
#endif

			for (auto & i : Window::WindowsContainer)
			{
				i.second->ProcessEvents(&event);
			}


			switch (event.type)
			{
			case (SDL_QUIT):
				ProceedToSafeClose();
				break;
			case (SDL_DROPFILE):
			{
				static int filesCount = 0;
				filesCount++;
				if (State::CheckState(State::FilesLoaded) == false)
					break;
				State::ResetState(State::AddedByFileBrowser);

				State::OnFileAddition();
				Audio::DroppedItemsCount++;
#ifdef _WIN32_
				std::string p(event.drop.file);
				
				Audio::SaveDroppedPath(p);
				//Audio::PushToPlaylistTest(p);
				
#else
				MP::PushToPlaylist(event.drop.file);
#endif
				SDL_free(SDL_GetClipboardText());
				SDL_free(event.drop.file);
				break;
			}
			case (SDL_DROPCOMPLETE):
				//Audio::timeCount = SDL_GetTicks();
				fileLoadThread = new std::thread(Audio::OnDropComplete);
				fileLoadThread->detach();
				delete fileLoadThread;
				break;
			case (SDL_MOUSEWHEEL):
				UpdateScrollPosition(event.wheel.x, event.wheel.y);
				break;
			case (SDL_MOUSEMOTION):
				UpdateMousePosition(event.motion.x, event.motion.y);
				break;
			}

			int wmId, wmEvent;

			if (event.type == SDL_SYSWMEVENT)
			{
				switch (event.syswm.msg->msg.win.msg)
				{
				case (WM_USER + 1):
				{
					switch (LOWORD(event.syswm.msg->msg.win.lParam))
					{
					case WM_LBUTTONDBLCLK:
						SDL_ShowWindow(mdWindow);
						SDL_RestoreWindow(mdWindow);
						State::ResetState(State::Window::InTray);
						//Shell_NotifyIcon(NIM_DELETE, &icon);
						break;
					case WM_RBUTTONDOWN:
						SDL_GetWindowWMInfo(mdWindow, &wmInfo);
						SDL_VERSION(&wmInfo.version);

						if (SDL_GetWindowWMInfo(mdWindow, &wmInfo))
						{
							POINT lpClickPoint;
							UINT uFlag = MF_BYPOSITION | MF_STRING;
							GetCursorPos(&lpClickPoint);
							hPopMenu = CreatePopupMenu();
							InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, IDM_SHOW_DEMP, _T("Show demp"));
							InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));
							InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, IDM_EXIT, _T("Exit"));

							SetForegroundWindow(wmInfo.info.win.window);
							TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, wmInfo.info.win.window, NULL);
						}
						break;
					case WM_MOUSEMOVE:
						State::SetState(State::Window::MouseOnTrayIcon);
						if (Window::TrayIconProperties.m_TrayPos == glm::ivec2() ||
							App::Input::GetGlobalMousePosition().y < Window::TrayIconProperties.m_TrayPos.y ||
							App::Input::GetGlobalMousePosition().x < Window::TrayIconProperties.m_TrayPos.x)
						{
							Window::TrayIconProperties.m_TrayPos = App::Input::GetGlobalMousePosition();
						}
						break;
					}
				}
				break;
				case WM_COMMAND:
					wmId = LOWORD(event.syswm.msg->msg.win.wParam);
					// Parse the menu selections:
					switch (wmId)
					{
					case IDM_SHOW_DEMP:
						Window::ShowWindow();
						Window::RestoreWindow();
						State::ResetState(State::Window::InTray);
						break;
					case IDM_EXIT:
						Shell_NotifyIcon(NIM_DELETE, &icon);
						AppExit();
						break;
					}
					break;
				
				case WM_NCXBUTTONUP:
					md_log("dragging");
					break;
				}

			}

			if (event.type == SDL_WINDOWEVENT && event.window.windowID == mdWindowID)
			{
				switch (event.window.event)
				{
				case (SDL_WINDOWEVENT_FOCUS_GAINED):
					State::SetState(State::Window::HasFocus);
					break;
				case (SDL_WINDOWEVENT_FOCUS_LOST):
					State::ResetState(State::Window::HasFocus);
					break;
				case (SDL_WINDOWEVENT_ENTER):
					State::SetState(State::Window::MouseEnter);
					break;
				case (SDL_WINDOWEVENT_LEAVE):
					State::ResetState(State::Window::MouseEnter);;
					break;
				case(SDL_WINDOWEVENT_MINIMIZED):
					if(State::CheckState(State::Window::InTray) == false)
						State::SetState(State::Window::Minimized);
					break;
				case (SDL_WINDOWEVENT_RESTORED):
					State::ResetState(State::Window::Minimized);
					break;
				case (SDL_WINDOWEVENT_HIDDEN):
					break;
				case (SDL_WINDOWEVENT_SHOWN):
					break;

				}
			}

			if (Window::mdOptionsWindow != NULL)
			{
				if (event.type == SDL_WINDOWEVENT && event.window.windowID == Window::mdOptionsWindow->GetWindowID())
				{

					switch (event.window.event)
					{
					case (SDL_WINDOWEVENT_FOCUS_GAINED):
						State::SetState(State::OptionWindow::HasFocus);
						break;
					case (SDL_WINDOWEVENT_FOCUS_LOST):
						State::ResetState(State::OptionWindow::HasFocus);;
						break;

					}
				}
			}
		}

			const u8* current_keystate = SDL_GetKeyboardState(NULL);
			mdEngine::UpdateKeyState(current_keystate);

			const u32 current_mousestate = SDL_GetMouseState(NULL, NULL);
			mdEngine::UpdateMouseState(current_mousestate);

			UpdateRelativeMousePosition();


			if (mdIsRunning == true)
			{
				UpdateWindowSize();
				mdApplicationHandler->OnRealtimeUpdate();
			}
			
			for(auto & i : Window::WindowsContainer)
				i.second->Update();

			Window::UpdateWindows();

			if (State::CheckState(State::Window::Minimized) == false ||
				State::CheckState(State::Window::InTray) == false ||
				State::CheckState(State::AudioChangedInTray) == true)
			{
				SDL_GL_MakeCurrent(mdWindow, gl_context);
				//glClearColor(Color::TransparentClearColor.x, Color::TransparentClearColor.y, Color::TransparentClearColor.z, 1.f);
				glClearColor(Color::TransparentClearColor.r, Color::TransparentClearColor.g, Color::TransparentClearColor.b, 1.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, mdCurrentWindowWidth, mdCurrentWindowHeight);

				Graphics::UpdateGraphics();

				Graphics::Shader::shaderDefault->use();
				Graphics::Shader::shaderDefault->setMat4("projection", *Graphics::GetProjectionMatrix());

				Graphics::RenderGraphics();
				mdApplicationHandler->OnRealtimeRender();
#ifdef _DEBUG_
				glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
				
#endif
				SDL_GL_SwapWindow(mdWindow);
			}
			// Render options window
			State::SetState(State::AppStarted);

			for (auto & i : Window::WindowsContainer)
			{
				if (i.second->IsActive() == true)
				{
					i.second->Render();
				}
			}

			if (State::IsBackgroundModeActive() == true)
			{
				MP::Data::UpdateFPS(1.f);
				f32 frameTicks = capTimer.GetTicks();
				if (frameTicks < MP::Data::_SCREEN_TICK_PER_FRAME)
				{
					SDL_Delay(MP::Data::_SCREEN_TICK_PER_FRAME - frameTicks);
					//md_log(MP::Data::_SCREEN_TICK_PER_FRAME - frameTicks);
				}
			}
			else
			{
				MP::Data::UpdateFPS(MP::Data::_SCREEN_FPS);
			}

			if (State::HighFPSMode() == false)
			{
				f32 frameTicks = capTimer.GetTicks();
				if (frameTicks < MP::Data::_SCREEN_TICK_PER_FRAME)
				{
					SDL_Delay(MP::Data::_SCREEN_TICK_PER_FRAME - frameTicks);
					//md_log(MP::Data::_SCREEN_TICK_PER_FRAME - frameTicks);
				}
			}
		}
	}

void mdEngine::StopRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	mdIsRunning = false;
}

void mdEngine::CloseRealtimeApplication(mdEngine::App::ApplicationHandlerInterface& applicationHandler)
{
	/* CLEAR AND FREE UP THE MEMORY */
	MP::Config::SaveToConfig();
	mdApplicationHandler->OnWindowClose();
	//MP::UI::GetOptionsWindow()->Free();
	Graphics::CloseGraphics();

#ifdef _DEBUG_
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
#endif

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(mdWindow);

#ifdef _WIN32_
	Shell_NotifyIcon(NIM_DELETE, &icon);

	SetWindowLong(hwnd, GWL_EXSTYLE,
		GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
	// Ask the window and its children to repaint
	RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME |
		RDW_ALLCHILDREN);
#else

#endif
	TTF_Quit();
	BASS_Free();
	SDL_Quit();

	mdIsRunning = false;
	mdHasApplication = false;
	mdApplicationHandler = nullptr;
}

void mdEngine::AppExit(b8 exitOnError)
{
	mdAppClosing = true;

	State::SetState(State::Window::Exit);

	if(State::CheckState(State::FilesAddedInfoNotLoaded) == true)
		State::SetState(State::SafeExitPossible);

	while(State::CheckState(State::SafeExitPossible) == true) { }

	if(exitOnError == true)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Missing file",
			"File is missing. Please reinstall the program.",
			NULL);
	mdIsRunning = false;
}


b8 mdEngine::IsAppClosing()
{
	return mdAppClosing;
}

void mdEngine::SetWindowProperties(const App::WindowProperties& windowProperties)
{
	mdActualWindowWidth = windowProperties.m_WindowWidth;
	mdActualWindowHeight = windowProperties.m_WindowHeight;
}

void mdEngine::Window::HideToTray()
{
#ifdef _WIN32_

	//SDL_GL_DeleteContext(gl_context);
	State::SetState(State::Window::InTray);
	MinimizeWindow();
	HideWindow();

	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
#endif
}

void mdEngine::Window::BackFromTray()
{

}

void mdEngine::Window::MinimizeWindow()
{
	SDL_MinimizeWindow(mdWindow);
}

void mdEngine::Window::ShowWindow()
{
	SDL_ShowWindow(mdWindow);
}

void mdEngine::Window::HideWindow()
{
	SDL_HideWindow(mdWindow);
}

void mdEngine::Window::RestoreWindow()
{
	SDL_RestoreWindow(mdWindow);
}

void mdEngine::Window::MaximizeWindow()
{
	SDL_MaximizeWindow(mdWindow);
}

SDL_Window* mdEngine::Window::GetSDLWindow()
{
	return mdWindow;
}

HWND mdEngine::Window::GetHWNDWindow()
{
	return hwnd;
}

SDL_GLContext* mdEngine::Window::GetMainWindowContext()
{
	return &gl_context;
}

void mdEngine::Window::SetWindowTitle(const b8& windowTitle)
{
	/* set window title sdl */
}

void mdEngine::Window::SetWindowPos(s32 x, s32 y)
{
	SDL_SetWindowPosition(mdWindow, x, y);
}

glm::vec2 mdEngine::Window::GetWindowPos()
{
	s32 x, y;
	SDL_GetWindowPosition(mdWindow, &x, &y);;

	return glm::vec2(x, y);
}

glm::vec2 mdEngine::Window::GetWindowPos(SDL_Window* window)
{
	s32 x, y;
	SDL_GetWindowPosition(window, &x, &y);;

	return glm::vec2(x, y);
}

void mdEngine::Window::SetWindowSize(s32 w, s32 h)
{
	SDL_SetWindowSize(mdWindow, w, h);
}

void mdEngine::Window::GetWindowSize(s32* w, s32* h)
{
	SDL_GetWindowSize(mdWindow, w, h);
}

glm::vec2 mdEngine::Window::GetWindowSize()
{
	s32 w, h;
	SDL_GetWindowSize(mdWindow, &w, &h);
	return glm::vec2(w, h);
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

b8 mdEngine::Window::CheckWindowSize()
{
	if (WindowProperties.m_ApplicationHeight > WindowProperties.m_WindowHeight)
	{
		WindowProperties.m_ApplicationHeight = WindowProperties.m_WindowHeight;
		return true;
	}

	return false;
}