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
		s16 mWindowWidth;
		s16 mWindowHeight;
		s16 mWindowPositionX;
		s16 mWindowPositionY;
		WindowMode mWindowMode;
		bool mVerticalSync;

		WindowProperties(void);

		//WindowProperties(const WindowMode& windowMode);

	};


	void WindowMovableBar(MP::UI::Movable* bar);

	void ProcessButtons(MP::UI::Button* button);
}
}

#endif // !MD_APPLICATION_WINDOW_H
