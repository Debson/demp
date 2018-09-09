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
		const s32 _SCREEN_FPS = 60;
		const s32 _SCREEN_TICK_PER_FRAME = 1000 / _SCREEN_FPS;
	}

	enum class WindowMode
	{
		Windowed,
		FullScreen,
	};


	struct WindowProperties
	{
		s32 mWindowWidth;
		s32 mWindowHeight;
		s32 mApplicationHeight;
		s32 mStartApplicationHeight;
		s32 mWindowPositionX;
		s32 mWindowPositionY;
		s32 mDeltaHeightResize;
		WindowMode mWindowMode;
		
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
