#include "music_player_state.h"


namespace mdEngine
{
	b8 State::MusicFilesLoaded(false);
	b8 State::MusicFilesInfoLoaded(false);

	b8 State::IsPlaylistEmpty(true);

	// Restarted to false every frame
	b8 State::PathLoadedFromFile(false);

	//
	b8 State::IsDeletionFinished(false);

	// Flags that NEED to be restarted at end of the frame
	void State::ResetStateFlags()
	{
		PathLoadedFromFile = false;
	}
}