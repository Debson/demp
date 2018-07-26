#include "realtime_application.h"

#include "realtime_system_application.h"
#include "application_window.h"


namespace mdEngine
{
	const MP::WindowProperties mdDefaultWindowProperties;
	u16 mdActualWindowWidth(mdDefaultWindowProperties.mWindowWidth);
	u16 mdActualWindowHeight(mdDefaultWindowProperties.mWindowHeight);

	namespace MP
	{

		RealtimeApplication::RealtimeApplication(MP::ApplicationHandlerInterface& applicationHandler) : mApplicationHandler(applicationHandler)
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
			SetWindowTitle(windowTitle);
		}
	}
}