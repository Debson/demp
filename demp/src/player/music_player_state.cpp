#include "music_player_state.h"

#include <iostream>

namespace mdEngine
{
	u64 State::m_currentState = State::None | State::PlaylistEmpty;


	void State::ResetStateFlags()
	{
		State::ResetState(State::PathLoadedFromFileVolatile);
	}

	void State::ResetMusicPlayerState()
	{
		ResetState(AudioAdded);
		ResetState(AudioDeleted);
		ResetState(AudioChosen);
		ResetState(AudioChanged);
		ResetState(ContainersResized);
		ResetState(FileDropped);
		ResetState(AudioHidden);
	}

	void State::StartNewFrame()
	{
		ResetState(AudioAdded);
		ResetState(AudioDeleted);
		ResetState(AudioChosen);
		ResetState(AudioChanged);
		ResetState(ContainersResized);
		ResetState(FileDropped);
		ResetState(AudioHidden);
		ResetState(Window::PositionChanged);
		ResetState(State::DeletionFinished);
		State::ResetState(State::PlaylistMovement);

	}
}