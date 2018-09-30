#include "music_player_state.h"

#include <iostream>

namespace mdEngine
{
	u64 State::m_currentState = State::None | State::PlaylistEmpty;


	void State::ResetStateFlags()
	{
		State::ResetState(State::InitialLoadFromFile);
	}

	void State::ResetStateOnUnsupportedFormat()
	{
		SetState(OldAudioObjectsSaved);
		ResetState(FilesDroppedNotLoaded);
		State::ResetState(State::FileDropped);
		State::ResetState(State::FilesAddedInfoNotLoaded);
	}

	void State::ResetMusicPlayerState()
	{
		ResetState(AudioAdded);
		ResetState(AudioDeleted);
		ResetState(AudioChosen);
		ResetState(AudioChanged);
		ResetState(ContainersResized);
		//ResetState(FileDropped);
		ResetState(AudioHidden);
	}

	b8 State::IsBackgroundModeActive()
	{
		return	(CheckState(Window::InTray) == true ||
			CheckState(Window::Minimized) == true) &&
			CheckState(Window::MouseOnTrayIcon) == false &&
			CheckState(AudioChangedInTray) == false;
	}

	void State::StartNewFrame()
	{
		ResetState(AudioAdded);
		ResetState(AudioDeleted);
		ResetState(AudioChosen);
		ResetState(AudioChanged);
		ResetState(ContainersResized);
		ResetState(AudioHidden);
		ResetState(Window::PositionChanged);
		ResetState(State::DeletionInProgress);
		State::ResetState(State::PlaylistMovement);

	}
}