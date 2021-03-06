#include "realtime_application.h"

#include "realtime_system_application.h"



namespace mdEngine
{
	//const App::WindowProperties mdDefaultWindowProperties;

	//s32 mdActualWindowWidth(mdDefaultWindowProperties.mWindowWidth);
	//s32 mdActualWindowHeight(mdDefaultWindowProperties.mWindowHeight);

	namespace App
	{

		RealtimeApplication::RealtimeApplication(App::ApplicationHandlerInterface& applicationHandler) : mApplicationHandler(applicationHandler)
		{

			// add implementation??
		}

		RealtimeApplication::~RealtimeApplication()
		{

		}

		void RealtimeApplication::Open()
		{
			OpenRealtimeApplication(mApplicationHandler);
		}

		void RealtimeApplication::Run()
		{
			RunRealtimeApplication(mApplicationHandler);
		}

		void RealtimeApplication::Stop()
		{
			StopRealtimeApplication(mApplicationHandler);
		}

		void RealtimeApplication::Close()
		{
			CloseRealtimeApplication(mApplicationHandler);
		}

		void RealtimeApplication::SetWindowTitle(const char* windowTitle)
		{
			mdEngine::Window::SetWindowTitle(windowTitle);
		}

		void RealtimeApplication::SetWindowProperties(const WindowProperties& windowProperties)
		{
			mdEngine::SetWindowProperties(windowProperties);
		}
	}
}