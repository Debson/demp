#pragma once
#ifndef MD_APPLICATION_WINDOW_H

#include "../utility/md_types.h"
#include "system_application_input.h"
#include "../interface/md_interface.h"

namespace mdEngine
{
namespace App
{
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
		s32 mApplicationWidth;
		s32 mStartApplicationHeight;
		s32 mWindowPositionX;
		s32 mWindowPositionY;
		s32 mDeltaHeightResize;
		WindowMode mWindowMode;
		
		bool mVerticalSync;

		WindowProperties(void);

	};

	void InitializeConfig();

	void SetButtonCheckBounds(s32 low, s32 high);

	void ProcessMovable(Interface::Movable* bar);

	void ProcessMovable(Interface::Movable* bar, SDL_Window* window, std::vector<Interface::Button*>* buttonCon);

	void ProcessButton(std::shared_ptr<Interface::Button> button);

	void ProcessButton(Interface::Button* button);

	void ProcesPlaylistButton(std::shared_ptr<Interface::PlaylistItem> button);

	void ProcessResizableTop(Interface::Resizable* barTop, Interface::Resizable* barBottom);

	void ProcessResizableBottom(Interface::Resizable* barBottom, Interface::Resizable* barTop);
}
}

#endif // !MD_APPLICATION_WINDOW_H
