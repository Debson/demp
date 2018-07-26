#include "application_window.h"


namespace mdEngine
{
	namespace MP
	{

		WindowProperties::WindowProperties() :
			mWindowWidth(800),
			mWindowHeight(600),
			mWindowPositionX(100),
			mWindowPositionY(100),
			mWindowMode(WindowMode::Windowed),
			mVerticalSync(true)
		{ }

	}
}
