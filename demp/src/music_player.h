#pragma once
#ifndef MD_MUSIC_PLAYER_H
#define MD_MUSIC_PLAYER_H

//#include <SDL_mixer.h>
#include <bass.h>
#include <SDL.h>
#include "music_player_settings.h"

#include "system_music_player.h"

#include "md_types.h"

namespace mdEngine
{
namespace Application
{
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
		b8 load(const char* songPath, u32 id, MP::Playlist::SongState state);
		b8 update(const char* songPath);

		HMUSIC& get();
	};
	
	
	class MusicPlayer
	{
	public:
		virtual ~MusicPlayer();
		

		virtual void Start();
		virtual void Update();
		virtual void Close();
	private:
	};

}
}

#endif // !MD_MUSCI_PLAYER_H
