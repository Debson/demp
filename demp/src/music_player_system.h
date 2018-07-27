#pragma once
#ifndef MD_SYSTEM_MUSIC_PLAYER_H
#define MD_SYSTEM_MUSIC_PLAYER_H

#include <vector>

#include "utf8_to_utf16.h"
#include "system_application_input.h"
#include "music_player_playlist.h"


namespace mdEngine
{
	namespace MP
	{
		void Open(void);

		void UpdateInput(void);

		void UpdateLogic(void);

		void Render();

		void PushToPlaylist(std::wstring path);

	}
}

#endif // !MD_SYSTEM_MUSIC_PLAYER_H
