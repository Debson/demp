#include "application_window.h"


mdEngine::Application::WindowProperties::WindowProperties() :
	mWindowWidth(800),
	mWindowHeight(600),
	mWindowPositionX(100),
	mWindowPositionY(100),
	mWindowMode(WindowMode::Windowed),
	mVerticalSync(true)
{ }
