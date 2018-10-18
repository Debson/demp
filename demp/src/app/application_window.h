#pragma once
#ifndef MD_APPLICATION_WINDOW_H

#include "../utility/md_types.h"
#include "system_application_input.h"
#include "../interface/md_interface.h"

namespace mdEngine
{
namespace App
{
	struct WindowProperties
	{
		s32 m_WindowWidth;
		s32 m_WindowHeight;
		s32 m_ApplicationHeight;
		s32 m_ApplicationWidth;
		s32 m_StartApplicationHeight;
		s32 m_WindowPositionX;
		s32 m_WindowPositionY;
		s32 m_WindowHeightBeforeMaximize;
		b8 m_IsMaximized;
		b8 mVerticalSync;

		WindowProperties(void);

	};

	struct MonitorProperties
	{
		s32 m_MonitorHeight;
		s32 m_MonitorWidth;
		s32 m_TaskBarHeight;
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
