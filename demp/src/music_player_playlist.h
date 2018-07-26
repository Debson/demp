#pragma once
#ifndef MUSIC_PLAYER_PLAYLIST_H
#define MUSIC_PLAYER_PLAYLIST_H

#include <vector>
#include <bass.h>

#include "music_player_settings.h"
#include "system_application_input.h"
#include "md_types.h"


namespace mdEngine
{
namespace MP
{
	namespace Playlist
	{
		enum SongState
		{
			mNext,
			mCurrent,
			mPrevious
		};

		struct SongObject
		{
			const char* mPath;
			char* mData;
			u32 mSize;
			HSTREAM mMusic;
			s64 mID;

			SongObject();
			~SongObject();

			b8 init(const char* songPath);
			b8 load(const char* songPath, u32 id, SongState state);
			b8 update(const char* songPath);

			HMUSIC& get();
		};

		extern SongObject RamLoadedSong;
		extern PathContainer mdPathContainer;

		void NextMusic();

		void PreviousMusic();

		void PlayMusic();

		void PauseMusic();

		void StopMusic();

		void IncreaseVolume(MP::InputEvent event);

		void LowerVolume(MP::InputEvent event);

		void RewindMusic(s32 pos);

		void UpdatePlaylist();

		void UpdateMusic();

		void SetMusicVolume(f32 vol);
	}
}
}
#endif // !MUSIC_PLAYER_PLAYLIST_H