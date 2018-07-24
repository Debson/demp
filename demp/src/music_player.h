#pragma once
#ifndef MD_MUSIC_PLAYER_H
#define MD_MUSIC_PLAYER_H

#include <SDL_mixer.h>
#include <SDL.h>

#include "md_types.h"

namespace mdEngine
{
namespace Application
{
	struct SongObject
	{
		const char* mPath;
		Mix_Music* mMusic;
		b8 mSelected;
		s64 id;

		SongObject();
		~SongObject();

		b8 selected();
		void select();
		void unselect();

		b8 load(const char* path);
		Mix_Music* get();
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
