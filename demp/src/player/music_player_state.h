#ifndef MUSIC_PLAYER_STATE_H
#define MUSIC_PLAYER_STATE_H

#include "../utility/md_types.h"
#include "../utility/md_util.h"


namespace mdEngine
{
	namespace State
	{
		enum AudioFilesState: u32
		{
			None						= (1u << 0),
			PlaylistEmpty				= (1u << 1),
			PlaylistFilled				= (1u << 2),
			AudioAdded					= (1u << 3),
			AudioDeleted				= (1u << 4),
			AudioChosen					= (1u << 5),
			AudioChanged				= (1u << 6),
			FilesLoaded					= (1u << 7),
			FilesInfoLoaded				= (1u << 8),
			PathLoadedFromFile			= (1u << 9),
			PathLoadedFromFileVolatile	= (1u << 10),
			DeletionFinished			= (1u << 11)

		};

		enum MusicPlayerState : u32
		{
			FileDropped			= (1u << 12),
			ContainersResized	= (1u << 13)
		};

		namespace Window
		{
			enum WindowEvent : u32
			{
				HasFocus	= (1u << 14),
				Resized		= (1u << 15),
				Minimized	= (1u << 16),
				Shown		= (1u << 17),
				Hidden		= (1u << 18),
				Exposed		= (1u << 19),
				MouseEnter	= (1u << 20),
				MouseLeave	= (1u << 21)
			};
		};

		extern u32 m_currentState;

		TEMPLATE void SetState(T state)
		{
			m_currentState |= state;
		}

		TEMPLATE b8 CheckState(T state)
		{
			return (m_currentState & state);
		}

		TEMPLATE void ResetState(T state)
		{
			m_currentState &= ~state;
		}

		// Reset all flags associated with Music player state, and set 
		void ResetMusicPlayerState();


		// Flags that NEED to be restarted at end of the frame
		void ResetStateFlags();
	}
}


#endif // !MUSIC_PLAYER_STATE_H

