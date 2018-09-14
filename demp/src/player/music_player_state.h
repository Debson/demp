#ifndef MUSIC_PLAYER_STATE_H
#define MUSIC_PLAYER_STATE_H

#include "../utility/md_types.h"
#include "../utility/md_util.h"


namespace mdEngine
{
	namespace State
	{
		enum AudioFilesState: u64
		{
			None						= (1llu << 0),
			PlaylistEmpty				= (1llu << 1),
			PlaylistFilled				= (1llu << 2),
			AudioAdded					= (1llu << 3),
			AudioDeleted				= (1llu << 4),
			AudioChosen					= (1llu << 5),
			AudioChanged				= (1llu << 6),
			AudioHidden					= (1llu << 7),
			AudioPlayStarted			= (1llu << 24),
			FilesLoaded					= (1llu << 8),
			FilesInfoLoaded				= (1llu << 9),
			PathLoadedFromFile			= (1llu << 10),
			PathLoadedFromFileVolatile	= (1llu << 11),
			DeletionFinished			= (1llu << 12),
			ShuffleAfterLoad			= (1llu << 30),
			ShuffleAfterAddition		= (1llu << 33),
			CurrentlyPlayingDeleted		= (1llu << 34)

		};

		enum MusicPlayerState : u64
		{
			FileDropped				= (1llu << 13),
			ContainersResized		= (1llu << 14),
			PlaylistMovement		= (1llu << 15),
			VolumeChanged			= (1llu << 25),
			CrossfadeEnabled		= (1llu << 26),
			PlaylistRolling			= (1llu << 28),
			OnExitMinimizeToTray	= (1llu << 29),
			InitMusicLoad			= (1llu << 31),
			InitialLoadFromFile		= (1llu << 32)
		};

		namespace Window
		{
			enum WindowEvent : u64
			{
				HasFocus		= (1llu << 16),
				Resized			= (1llu << 17),
				Minimized		= (1llu << 18),
				Shown			= (1llu << 19),
				Hidden			= (1llu << 20),
				Exposed			= (1llu << 21),
				MouseEnter		= (1llu << 22),
				MouseLeave		= (1llu << 23),
				PositionChanged	= (1llu << 27)
			};
		};

		extern u64 m_currentState;

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

		// States that should be reseted every frame
		void StartNewFrame();
	}
}


#endif // !MUSIC_PLAYER_STATE_H

