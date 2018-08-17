#pragma once
#ifndef MD_APPLICATION_WINDOW_H

#include "../utility/md_types.h"
#include "system_application_input.h"
#include "../interface/md_interface.h"

namespace mdEngine
{
namespace App
{
	//TODO move to settings
	namespace Data
	{
		const s32 _SCREEN_FPS = 120;
		const s32 _SCREEN_TICK_PER_FRAME = 1000 / _SCREEN_FPS;
	}

	enum class WindowMode
	{
		Windowed,
		FullScreen,
	};

	enum WindowEvent : u8
	{
		kFocusGained	= SDL_WINDOWEVENT_FOCUS_GAINED,
		kFocusLost		= SDL_WINDOWEVENT_FOCUS_LOST,
		kResize			= SDL_WINDOWEVENT_RESIZED,
		kMinimized		= SDL_WINDOWEVENT_MINIMIZED,
		kShown			= SDL_WINDOWEVENT_SHOWN,
		kHidden			= SDL_WINDOWEVENT_HIDDEN,
		kExposed		= SDL_WINDOWEVENT_EXPOSED,
		kEnter			= SDL_WINDOWEVENT_ENTER,
		kLeave			= SDL_WINDOWEVENT_LEAVE,

		kNone
	};

	struct WindowProperties
	{
		s32 mWindowWidth;
		s32 mWindowHeight;
		s32 mApplicationHeight;
		s32 mWindowPositionX;
		s32 mWindowPositionY;
		s32 mDeltaHeightResize;
		WindowMode mWindowMode;
		WindowEvent mActualWindowEvent;		// All music player events, like music changed etc. (this should be switched with mPlayerWindowevent)
		WindowEvent mPlayerWindowEvent;		// Focus gained or regained
		WindowEvent mMouseWindowEvent;		// Enter of leave, manages update of all controls
		
		bool mVerticalSync;

		WindowProperties(void);

	};


	void SetButtonCheckBounds(s32 low, s32 high, b8 val);

	void ProcessMovable(Interface::Movable* bar);

	void ProcessButton(Interface::Button* button);

	void ProcessResizable(Interface::Resizable* bar);
}
}

#endif // !MD_APPLICATION_WINDOW_H
