#ifndef MUSIC_PLAYER_STATE_H
#define MUSIC_PLAYER_STATE_H

#include "../utility/md_types.h"


namespace mdEngine
{
	namespace State
	{
		extern b8 MusicFilesLoaded;
		extern b8 MusicFilesInfoLoaded;
		extern b8 IsPlaylistEmpty;
		extern b8 PathLoadedFromFile;
		extern b8 IsDeletionFinished;

		void ResetStateFlags();
	}
}


#endif // !MUSIC_PLAYER_STATE_H

