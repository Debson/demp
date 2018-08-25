#pragma once
#ifndef REALTIME_SYSTEM_APPLICATION_HH
#define REALTIME_SYSTEM_APPLICATION_H

#include "system_application_input.h"
#include "../utility/md_types.h"
#include "application_handler_interface.h"
#include "../utility/utf8_to_utf16.h"


namespace mdEngine
{
	namespace App
	{
		class ApplicationHandlerInterface;
		struct WindowProperties;

	}
}

namespace mdEngine
{
	void OpenRealtimeApplication(App::ApplicationHandlerInterface& applicationHandler);
	void RunRealtimeApplication(App::ApplicationHandlerInterface& applicationHandler);
	void StopRealtimeApplication(App::ApplicationHandlerInterface& applicationHandler);
	void CloseRealtimeApplication(App::ApplicationHandlerInterface& applicationHandler);
	void SetWindowProperties(const App::WindowProperties &windowProperties);

	void AppExit();

	namespace Window
	{
		extern App::WindowProperties windowProperties;

		HWND GetHWNDWindow();
		void SetWindowTitle(const b8& windowName);
		void SetWindowPos(s32 x, s32 y);
		void SetWindowSize(s32 w, s32 h);
		void GetWindowSize(s32* w, s32* h);
		void GetWindowScale(f32* scaleX, f32* scaleY);
		void GetWindowPos(s32* x, s32* y);
		void UpdateViewport(s32 w, s32 h);
	}
}

#endif // !REALTIME_SYSTEM_APPLICATION_H