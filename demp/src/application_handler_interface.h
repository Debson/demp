#pragma once
#ifndef MD_APPLICATION_HANDLER_INTERFACE_H
#define MD_APPLICATION_HANDLER_INTERFACE_H

namespace mdEngine
{
	namespace MP
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

		private:
		};
	}
}

#endif // !MD_APPLICATION_HANDLER_INTERFACE_H
