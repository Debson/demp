#pragma once
#ifndef MD_MUSIC_PLAYER_H
#define MD_MUSIC_PLAYER_H

#include <bass.h>
#include <SDL.h>

#include "../utility/md_types.h"

namespace mdEngine
{
	namespace MP
	{
		enum MusicPlayerState
		{
			kIdle,
			kMusicChanged,
			kResized,
			kMusicChosen,
			kMusicDeleted,
			kMusicAdded
		};

		extern MusicPlayerState musicPlayerState;

		class MusicPlayer
		{
		public:
			virtual ~MusicPlayer();
			
			virtual void Start();
			virtual void Update();
			virtual void Render();
			virtual void Close();
		private:
		};
}
}

#endif // !MD_MUSCI_PLAYER_H
