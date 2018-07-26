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
		void OpenMusicPlayer(void);

		void UpdateMusicPlayerInput(void);

		void UpdateMusicPlayerLogic(void);

		void PushToPlaylist(const char* path);

	}
}

#endif // !MD_SYSTEM_MUSIC_PLAYER_H
