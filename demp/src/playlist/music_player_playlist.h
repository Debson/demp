#pragma once
#ifndef MUSIC_PLAYER_PLAYLIST_H
#define MUSIC_PLAYER_PLAYLIST_H

#include <vector>
#include <bass.h>

#include "../audio/mp_audio.h"
#include "../settings/configuration.h"
#include "../settings/music_player_settings.h"
#include "../app/system_application_input.h"
#include "../utility/md_types.h"


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
			SongObject();
			~SongObject();

			char *m_Data;
			u32 m_MusicSize;
			HSTREAM m_MusicStream;
			s32 m_ID;


#ifdef _WIN32_
			b8 load(std::shared_ptr<Audio::AudioObject> audioObject);
			std::wstring m_Path;
#else
			b8 init(const char* songPath);
			b8 load(const char* songPath, u32 id, SongState state);
			const char* mPath;
#endif

			void Free();

			HMUSIC& get();
		};

		extern SongObject RamLoadedMusic;
		extern PathContainer mdPathContainer;

		void InitializeConfig();

		void Start();

		void NextMusic();

		void PreviousMusic();

		void PlayMusic();

		void PauseMusic();

		void StopMusic();

		void IncreaseVolume(App::InputEvent event);

		void LowerVolume(App::InputEvent event);

		void RewindMusic(s32 pos);

		void UpdatePlaylist();

		void UpdateMusic();

		void ReloadMusic(std::wstring path);

		void DeleteMusic(const std::vector<s32>* indexes);


		b8 IsLoaded();

		b8 IsPaused();

		b8 IsPlaying();

		// Returns playing state immidiately, without volume fade delay
		b8 IsChannelPlaying();

		b8 IsShuffleEnabled();

		b8 IsRepeatEnabled();

		std::wstring GetTitle(s32 id);

		s32 GetPreviousID();

		s32 GetNextID();

		/* Returns current song position in proper format (mm:ss) */
		std::string GetPositionInString();

		/* Returns current song position in seconds*/

		void SetVolume(f32 vol);

		f32 GetPosition();

		f32 GetVolume();

		void GetBitrate(f32* bitrate);

		void MuteVolume(b8 param);

		/* Returns music length in seconds*/
		f32 GetMusicLength();

#ifdef _DEBUG_
		s32 GetCurrentShufflePos();

		s32 GetShuffleContainerSize();

		void PrintShuffledPositions();

		void PrintLoadedPaths();
#endif

		void SetPosition(s32 pos);

		void RepeatMusic();

		void ShuffleMusic();

		void SetScrollVolumeStep(s8 step);

		void SetVolumeFadeTime(s32 vol);
	}
}
}
#endif // !MUSIC_PLAYER_PLAYLIST_H