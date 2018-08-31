#include "music_player_state.h"

#include <iostream>

namespace mdEngine
{
	u32 State::m_currentState = State::None | State::PlaylistEmpty;


	void State::ResetStateFlags()
	{
		State::ResetState(State::PathLoadedFromFileVolatile);
	}

	void State::ResetMusicPlayerState()
	{
		ResetState(Window::Resized);
		ResetState(AudioAdded);
		ResetState(AudioDeleted);
		ResetState(AudioChosen);
		ResetState(AudioChanged);
		ResetState(ContainersResized);
		ResetState(FileDropped);
	}
}