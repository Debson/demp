#pragma once
#ifndef MD_SYSTEM_MUSIC_PLAYER_H
#define MD_SYSTEM_MUSIC_PLAYER_H

#include <vector>

#include "../utility/utf8_to_utf16.h"
#include "../app/system_application_input.h"
#include "../playlist/music_player_playlist.h"


namespace mdEngine
{
	namespace MP
	{
		void OpenMusicPlayer(void);

		void UpdateInput(void);

		void UpdateLogic(void);

		void RenderMusicPlayer();

		void CloseMusicPlayer();

#ifdef _WIN32_
		void PushToPlaylist(std::wstring* path);
#else
		void PushToPlaylist(const char* path);
#endif

	}
}

#endif // !MD_SYSTEM_MUSIC_PLAYER_H
