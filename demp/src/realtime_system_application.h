#pragma once
#ifndef REALTIME_SYSTEM_APPLICATION_HH
#define REALTIME_SYSTEM_APPLICATION_H

#include "system_application_input.h"
#include "md_types.h"
#include "application_handler_interface.h"

namespace mdEngine
{
	namespace MP
	{
		class ApplicationHandlerInterface;
		struct WindowProporties;
	}
}

namespace mdEngine
{
	void OpenRealtimeApplication(MP::ApplicationHandlerInterface& applicationHandler);
	void RunRealtimeApplication(MP::ApplicationHandlerInterface& applicationHandler);
	void StopRealtimeApplication(MP::ApplicationHandlerInterface& applicationHandler);
	void CloseRealtimeApplication(MP::ApplicationHandlerInterface& applicationHandler);

	void SetWindowTitle(const char& windowName);
}

#endif // !REALTIME_SYSTEM_APPLICATION_H
