#pragma once
#ifndef MD_REALTIME_APPLICATION_H
#define MD_REALTIME_APPLICATION_H

#include "application_handler_interface.h"
#include "application_window.h"

namespace mdEngine
{
	namespace App
	{
		class RealtimeApplication
		{
		public:
			RealtimeApplication(ApplicationHandlerInterface& applicationHandler);

			~RealtimeApplication();

			void Open();

			void Run();

			void Stop();

			void Close();

			void SetWindowTitle(const char* windowTitle);

			void SetWindowProperties(const WindowProperties& windowProperties);

		private:
			ApplicationHandlerInterface& mApplicationHandler;
		};
	}
}

#endif // !MD_REALTIME_APPLICATION_H
