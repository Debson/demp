#pragma once
#ifndef MD_APPLICATION_WINDOW_H

#include "md_types.h"
#include "system_application_input.h"
#include "music_player_ui.h"

namespace mdEngine
{
namespace App
{
	enum class WindowMode
	{
		Windowed,
		FullScreen,
	};

	enum WindowEvent
	{
		kNone,
		kFocus,
		kResize,
		kMinimized,
		kExposed,
		kEnter,
		kLeave
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
