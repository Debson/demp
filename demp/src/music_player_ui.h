#pragma once
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include "../external/imgui/imgui.h"
#include "md_types.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		enum ButtonType
		{
			Exit,
			Minimalize,
			
			count
		};

		struct Movable
		{
			f32 xL;
			f32 yU;
			f32 xR;
			f32 yD;

			Movable();
			Movable(s32 xL, s32 yU, s32 xR, s32 yD);
		};

		struct Button
		{
			f32 xL;
			f32 yU;
			f32 xR;
			f32 yD;

			b8 isPressed;
			b8 isDown;

			Button();
			Button(ButtonType type, s32 xL, s32 yU, s32 xR, s32 yD);
		};


		extern ImVec4 ClearColor;

		void Start();

		void Update();

		void Render();

	}
}
}


#endif // !MUSIc_PLAYER_UI_H
