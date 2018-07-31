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
		FullScreen
	};

	struct WindowProperties
	{
		s32 mWindowWidth;
		s32 mWindowHeight;
		s32 mWindowPositionX;
		s32 mWindowPositionY;
		WindowMode mWindowMode;
		bool mVerticalSync;

		WindowProperties(void);

		//WindowProperties(const WindowMode& windowMode);

	};


	void ProcessMovable(MP::UI::Movable* bar);

	void ProcessButtons(MP::UI::Button* button);
}
}

#endif // !MD_APPLICATION_WINDOW_H
