#include "realtime_application.h"
#include "realtime_system_application.h"
#include "application_window.h"


namespace mdEngine
{
	const mdEngine::Application::WindowProperties mdDefaultWindowProperties;
	u16 mdActualWindowWidth(mdDefaultWindowProperties.mWindowWidth);
	u16 mdActualWindowHeight(mdDefaultWindowProperties.mWindowHeight);

	Application::RealtimeApplication::RealtimeApplication(Application::ApplicationHandlerInterface& applicationHandler) : mApplicationHandler(applicationHandler)
	{

		// add implementation??
	}
	
	Application::RealtimeApplication::~RealtimeApplication()
	{

	}

	void Application::RealtimeApplication::Open()
	{
		OpenRealtimeApplication(mApplicationHandler);
	}

	void Application::RealtimeApplication::Run()
	{
		RunRealtimeApplication(mApplicationHandler);
	}

	void Application::RealtimeApplication::Stop()
	{
		StopRealtimeApplication(mApplicationHandler);
	}

	void Application::RealtimeApplication::Close()
	{
		CloseRealtimeApplication(mApplicationHandler);
	}

	void Application::RealtimeApplication::SetWindowTitle(const char* windowTitle)
	{
		SetWindowTitle(windowTitle);
	}
}