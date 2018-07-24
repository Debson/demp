#pragma once
#ifndef MD_SYSTEM_MUSIC_PLAYER_H
#define MD_SYSTEM_MUSIC_PLAYER_H

#include "music_player.h"

namespace mdEngine
{
namespace MP
{
	namespace Playlist
	{
		void InitializePlaylist();

		void UpdatePlaylist();

		void UpdateMusic();

		void NextMusic();

		void PreviousMusic();

		void PlayMusic();

		void PauseMusic();

		void HaltMusic();

		void IncreaseVolume();

		void LowerVolume(); 

		void VolumeFadeIn(); // shouldnt be here

		void VolumeFadeOut(); // shouldnt be here

		void SetMusicVolume(s8 vol); // shouldnt be here



	}

	void OpenMusicPlayer(void);

	void UpdateMusicPlayerInput(void);

	void UpdateMusicPlayerLogic(void);

	void PushToPlaylist(const char* path);

}
}

#endif // !MD_SYSTEM_MUSIC_PLAYER_H
