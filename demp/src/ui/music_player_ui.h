#pragma once
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include <glm.hpp>
#include <SDL_ttf.h>
#include <GL/gl3w.h>

#include "../../external/imgui/imgui.h"

#include "../interface/md_interface.h"
#include "../interface/md_helper_windows.h"
#include "music_player_ui_input.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
	{

		typedef std::vector<std::pair<Input::ButtonType, Interface::Button*>>	ButtonContainer;
		typedef std::vector<Interface::Resizable*>								ResizableContainer;
		typedef std::vector<Interface::Movable*>								MovableContainer;

		
		extern ButtonContainer		mdButtonsContainer;
		extern ResizableContainer	mdResizableContainer;
		extern MovableContainer		mdMovableContainer;

		extern b8 fileBrowserActive;

		/* useless */
		extern ImVec4 ClearColor;

		void DeleteAllFiles();

		Window::OptionsWindow* GetOptionsWindow();

		void Start();

		void Update();

		void Render();

		void Close();
	}
}
}


#endif // !MUSIc_PLAYER_UI_H
