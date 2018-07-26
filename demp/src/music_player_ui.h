#pragma once
#ifndef MUSIC_PLAYER_UI_H
#define MUSIC_PLAYER_UI_H

#include "../external/imgui/imgui.h"

namespace mdEngine
{
namespace MP
{
	namespace UI
	{
		extern ImVec4 ClearColor;

		void Start();

		void Update();

		void Render();

	}
}
}


#endif // !MUSIc_PLAYER_UI_H
