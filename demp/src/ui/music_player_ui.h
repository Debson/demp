#pragma once
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include <glm.hpp>
#include <SDL_ttf.h>
#include <GL/gl3w.h>

#include "../../external/imgui/imgui.h"

#include "../interface/md_interface.h"
#include "music_player_ui_input.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		extern std::vector<Interface::Movable*> mdMovableContainer;
		extern std::vector<Interface::Resizable*> mdResizableContainer;
		extern std::vector<std::pair<Input::ButtonType, Interface::Button*>> mdButtonsContainer;


		extern b8 fileBrowserActive;

		/* useless */
		extern ImVec4 ClearColor;

		void Start();

		void Update();

		void Render();

		void Close();
	}
}
}


#endif // !MUSIc_PLAYER_UI_H
