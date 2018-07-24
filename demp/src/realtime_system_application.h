#pragma once
#ifndef REALTIME_SYSTEM_APPLICATION_HH
#define REALTIME_SYSTEM_APPLICATION_H

#include "system_application_input.h"
#include "md_types.h"
#include "application_handler_interface.h"

namespace mdEngine
{
	namespace Application
	{
		class ApplicationHandlerInterface;
		struct WindowProporties;
	}
}

namespace mdEngine
{
	void OpenRealtimeApplication(Application::ApplicationHandlerInterface& applicationHandler);
	void RunRealtimeApplication(Application::ApplicationHandlerInterface& applicationHandler);
	void StopRealtimeApplication(Application::ApplicationHandlerInterface& applicationHandler);
	void CloseRealtimeApplication(Application::ApplicationHandlerInterface& applicationHandler);

	void SetWindowTitle(const char& windowName);
}

#endif // !REALTIME_SYSTEM_APPLICATION_H
