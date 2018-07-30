#pragma once
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include "../external/imgui/imgui.h"
#include "md_types.h"

#include "music_player_ui_input.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		struct Movable
		{
			f32 xL;
			f32 yU;
			f32 xR;
			f32 yD;

			Movable();
			~Movable();
			Movable(s32 xL, s32 yU, s32 xR, s32 yD);
		};

		struct Button
		{
			s32 xL;
			s32 yU;
			s32 xR;
			s32 yD;

			b8 isPressed;
			b8 isReleased;
			b8 isDown;
			b8 hasFocus;

			Button();
			~Button();
			Button(Input::ButtonType type, s32 xL, s32 yU, s32 xR, s32 yD);
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
