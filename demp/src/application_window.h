#pragma once
#ifndef MD_APPLICATION_WINDOW_H

#include "md_types.h"
#include "system_application_input.h"
#include "music_player_ui.h"

namespace mdEngine
{
namespace App
{
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
		WindowEvent mActualWindowEvent;
		WindowEvent mPlayerWindowEvent;
		
		bool mVerticalSync;

		WindowProperties(void);

		//WindowProperties(const WindowMode& windowMode);

	};



	void ProcessMovable(MP::UI::Movable* bar);

	void ProcessButton(MP::UI::Button* button);

	void ProcessResizable(MP::UI::Resizable* bar);
}
}

#endif // !MD_APPLICATION_WINDOW_H
