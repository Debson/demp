#pragma once
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include "../external/imgui/imgui.h"
#include "md_types.h"
#include <glm.hpp>

#include "music_player_ui_input.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		namespace Data
		{
			extern glm::vec2 _MAIN_BACKGROUND_POS;
			extern glm::vec2 _MAIN_BACKGROUND_SIZE;

			extern glm::vec2 _MAIN_FOREGROUND_POS;
			extern glm::vec2 _MAIN_FOREGROUND_SIZE;

			extern glm::vec2 _VOLUME_BAR_BOUNDS_POS;
			extern glm::vec2 _VOLUME_BAR_BOUNDS_SIZE;

			extern glm::vec2 _VOLUME_BAR_MIDDLE_POS;
			extern glm::vec2 _VOLUME_BAR_MIDDLE_SIZE;

			extern glm::vec2 _UI_WINDOW_BAR_POS;
			extern glm::vec2 _UI_WINDOW_BAR_SIZE;

			extern glm::vec2 _EXIT_BUTTON_POS;
			extern glm::vec2 _EXIT_BUTTON_SIZE;

			extern glm::vec2 _MINIMIZE_BUTTON_POS;
			extern glm::vec2 _MINIMIZE_BUTTON_SIZE;

			extern glm::vec2 _STAY_ON_TOP_BUTTON_POS;
			extern glm::vec2 _STAY_ON_TOP_BUTTON_SIZE;

			extern glm::vec2 _PLAY_BUTTON_POS;
			extern glm::vec2 _PLAY_BUTTON_SIZE;

			extern glm::vec2 _NEXT_BUTTON_POS;
			extern glm::vec2 _NEXT_BUTTON_SIZE;

			extern glm::vec2 _PREVIOUS_BUTTON_POS;
			extern glm::vec2 _PREVIOUS_BUTTON_SIZE;

			extern glm::vec2 _SHUFFLE_BUTTON_POS;
			extern glm::vec2 _SHUFFLE_BUTTON_SIZE;

			extern glm::vec2 _REPEAT_BUTTON_POS;
			extern glm::vec2 _REPEAT_BUTTON_SIZE;

			extern glm::vec2 _DOT_BUTTON_STATE_SIZE;
		
		}

		struct Movable
		{
			Movable();
			Movable(glm::vec2 size, glm::vec2 pos);
			~Movable();

			glm::vec2 size;
			glm::vec2 pos;

		};

		struct Button
		{
			Button();
			Button(Input::ButtonType type, glm::vec2 size, glm::vec2 pos);
			~Button();
			
			glm::vec2 size;
			glm::vec2 pos;

			b8 isPressed;
			b8 isReleased;
			b8 isDown;
			b8 hasFocus;

		};


		extern std::vector<std::pair<Input::ButtonType, Button*>> mdButtonContainer;

		/* useless */
		extern ImVec4 ClearColor;

		void Start();

		void Update();

		void Render();

	}
}
}


#endif // !MUSIc_PLAYER_UI_H
