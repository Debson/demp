#pragma once
#ifndef MD_SYSTEM_MUSIC_PLAYER_H
#define MD_SYSTEM_MUSIC_PLAYER_H

#include <vector>

#include "system_application_input.h"


namespace mdEngine
{
namespace MP
{
	extern PathContainer mdPathContainer;

	namespace Playlist
	{
		enum SongState
		{
			mNext,
			mCurrent,
			mPrevious
		};

		void NextMusic();

		void PreviousMusic();

		void PlayMusic();

		void PauseMusic();

		void StopMusic();

		void IncreaseVolume(Application::InputEvent event);

		void LowerVolume(Application::InputEvent event);

		void RewindMusic(s32 pos);
	}

	void OpenMusicPlayer(void);

	void UpdateMusicPlayerInput(void);

	void UpdateMusicPlayerLogic(void);

	void PushToPlaylist(const char* path);

}
}

#endif // !MD_SYSTEM_MUSIC_PLAYER_H
