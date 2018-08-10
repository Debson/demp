#pragma once
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include <glm.hpp>
#include <SDL_ttf.h>
#include <GL/gl3w.h>
#include "../external/imgui/imgui.h"

#include "music_player_ui_input.h"
#include "interface/md_interface.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		extern std::vector<Interface::Movable*> mdMovableContainer;
		extern std::vector<Interface::PlaylistItem*> mdItemContainer;
		extern std::vector<Interface::Resizable*> mdResizableContainer;
		extern std::vector<std::pair<Input::ButtonType, Interface::Button*>> mdButtonsContainer;
		extern std::vector<std::pair<Input::ButtonType, Interface::Button*>> mdPlaylistButtonsContainer;


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
