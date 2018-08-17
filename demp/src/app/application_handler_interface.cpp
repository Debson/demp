#include "application_handler_interface.h"


namespace mdEngine
{
	namespace App
	{
		ApplicationHandlerInterface::~ApplicationHandlerInterface() { }

		void ApplicationHandlerInterface::OnWindowOpen() { }

		void ApplicationHandlerInterface::OnWindowClose() { }

		void ApplicationHandlerInterface::OnBecomeActive() { }

		void ApplicationHandlerInterface::OnBecomeInactive() { }

		void ApplicationHandlerInterface::OnRealtimeUpdate() { }

		void ApplicationHandlerInterface::OnRealtimeRender() { }

		void ApplicationHandlerInterface::CollectWindowProperties(WindowProperties& windowProperties) { }
	}
}