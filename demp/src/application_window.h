#pragma once
#ifndef MD_APPLICATION_WINDOW_H

#include "md_types.h"

namespace mdEngine
{
namespace MP
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

		WindowProperties(const WindowMode& windowMode);

	};
}
}

#endif // !MD_APPLICATION_WINDOW_H
