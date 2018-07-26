#pragma once
#ifndef MD_SYSTEM_MUSIC_PLAYER_H
#define MD_SYSTEM_MUSIC_PLAYER_H

#include <vector>

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

		void PushToPlaylist(const char* path);

	}
}

#endif // !MD_SYSTEM_MUSIC_PLAYER_H
