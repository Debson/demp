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
			AudioChangedInTray			= (1llu << 7),
			AudioHidden					= (1llu << 8),
			AudioPlayStarted			= (1llu << 9),
			FilesLoaded					= (1llu << 10),
			FilesInfoLoaded				= (1llu << 11),
			PathLoadedFromFile			= (1llu << 12),
			InitialLoadFromFile			= (1llu << 13),
			DeletionInProgress			= (1llu << 14),
			ShuffleAfterLoad			= (1llu << 15),
			ShuffleAfterAddition		= (1llu << 16),
			CurrentlyPlayingDeleted		= (1llu << 17),
			OldAudioObjectsSaved		= (1llu << 44)

		};

		enum MusicPlayerState : u64
		{
			FileDropped					= (1llu << 18),
			FilesDroppedNotLoaded		= (1llu << 42),
			FilesAddedInfoNotLoaded		= (1llu << 43),
			DropComplete				= (1llu << 43),
			ContainersResized			= (1llu << 19),
			PlaylistMovement			= (1llu << 20),
			VolumeChanged				= (1llu << 21),
			CrossfadeEnabled			= (1llu << 22),
			PlaylistRolling				= (1llu << 23),
			OnExitMinimizeToTray		= (1llu << 24),
			LoadMusicOnFileLoad				= (1llu << 25),
			UpdatePlaylistInfoStrings	= (1llu << 26),
			SafeExitPossible			= (1llu << 46),
			SortPathsOnNewFileLoad		= (1llu << 47),
			PathContainerSorted			= (1llu << 48),
			TerminateWorkingThreads		= (1llu << 49)
		};

		namespace Window
		{
			enum WindowEvent : u64
			{
				HasFocus		= (1llu << 27),
				Resized			= (1llu << 28),
				ResizedFromTop	= (1llu << 29),
				Minimized		= (1llu << 30),
				Shown			= (1llu << 31),
				Hidden			= (1llu << 32),
				Exposed			= (1llu << 33),
				MouseEnter		= (1llu << 34),
				MouseLeave		= (1llu << 35),
				PositionChanged	= (1llu << 36),
				InTray			= (1llu << 37),
				MouseOnTrayIcon = (1llu << 38),
				Exit			= (1llu << 45),
			};
		};

		namespace OptionWindow
		{
			enum WindowEvent : u64
			{
				HasFocus			= (1llu << 39),
				Minimized			= (1llu << 40),
				Shown				= (1llu << 41)
			};
		}

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

		void ResetStateOnLoadError();

		// Reset all flags associated with Music player state, and set 
		void ResetMusicPlayerState();

		b8 IsBackgroundModeActive();


		// Flags that NEED to be restarted at end of the frame
		void ResetStateFlags();

		// States that should be reseted every frame
		void StartNewFrame();
	}
}


#endif // !MUSIC_PLAYER_STATE_H

