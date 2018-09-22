#pragma once
#ifndef MUSIC_PLAYER_GRAPHICS_H
#define MUSIC_PLAYER_GRAPHICS_H
#include <glm.hpp>

#include "../utility/md_types.h"
#include "../utility/md_time.h"
#include "../interface/md_interface.h"

namespace mdEngine
{
namespace Graphics
{
	namespace MP
	{
		// TODO: It cant be an extern variable...
		extern Interface::TextBox m_AddFileTextBox;

		void InitializeConfig();

		void StartMainWindow();

		void UpdateMainWindow();

		void RenderMainWindow();

		void CloseMainWindow();

#ifdef _DEBUG_
		void PrintVisibleItemsInfo();
		
		void PrintAudioObjectInfo();

		void PrintIndexesToRender();
#endif

	}
}
}

#endif // !MUSIC_PLAYER_GRAPHICS_H
