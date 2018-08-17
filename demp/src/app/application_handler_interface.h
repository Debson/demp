#pragma once
#ifndef MD_APPLICATION_HANDLER_INTERFACE_H
#define MD_APPLICATION_HANDLER_INTERFACE_H

#include "application_window.h"

namespace mdEngine
{
	namespace App
	{

		class ApplicationHandlerInterface
		{
		public:
			virtual ~ApplicationHandlerInterface(void);

			virtual void OnWindowOpen(void);

			virtual void OnWindowClose(void);

			/* Invoked each time the window gains or regains focus*/
			virtual void OnBecomeActive(void);

			virtual void OnBecomeInactive(void);

			virtual void OnRealtimeUpdate(void);

			virtual void OnRealtimeRender();

			virtual void CollectWindowProperties(WindowProperties& windowProperties);

		private:
		};
	}
}

#endif // !MD_APPLICATION_HANDLER_INTERFACE_H
