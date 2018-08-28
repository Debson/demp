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

		// This flag is reseted every frame to false
		extern b8 PathLoadedFromFileVolatile;

		// This flag is set on start of the application and hold its value till file is loaded
		extern b8 PathLoadedFromFileConst;

		extern b8 IsDeletionFinished;

		// Flags that NEED to be restarted at end of the frame
		void ResetStateFlags();
	}
}


#endif // !MUSIC_PLAYER_STATE_H

