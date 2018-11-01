#include "music_player_state.h"

#include "../app/realtime_system_application.h"
#include "../app/input.h"

#include <iostream>

namespace mdEngine
{
	u64 State::m_currentState = State::None | State::PlaylistEmpty;


	void State::ResetStateFlags()
	{
		//State::ResetState(State::InitialLoadFromFile);
	}

	void State::ResetStateOnLoadError()
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
		/*if (CheckState(Window::InTray) == true &&
			App::Input::GetGlobalMousePosition().y > mdEngine::Window::MonitorProperties.m_MonitorHeight - mdEngine::Window::MonitorProperties.m_TaskBarHeight)
			return false;*/

		if (CheckState(Window::MouseOnTrayIcon) == true)
			return false;

		if (CheckState(AudioChangedInTray) == true)
			return false;

		if (CheckState(Window::InTray) == true)
			return true;

		if (CheckState(Window::Minimized) == true)
			return true;



		return false;
	}

	b8 State::HighFPSMode()
	{
		if (State::CheckState(State::PlaylistRolling) == true)
			return true;

		if (State::CheckState(State::Window::PositionChanged) == true)
			return true;

		if (State::CheckState(State::Window::Resized) == true)
			return true;

		if (State::CheckState(State::PlaylistMovement) == true)
			return true;

		if (State::CheckState(State::FilesDroppedNotLoaded) == true)
			return true;
		
		return false;
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
		ResetState(State::VolumeChangedOnce);
		//State::ResetState(State::PlaylistMovement);

	}

	void State::OnFileAddition()
	{
		if (CheckState(State::PathContainerSorted) == false)
			SetState(State::SortPathsOnNewFileLoad);

		SetState(State::FileDropped);
		ResetState(State::InitialLoadFromFile);
		CheckState(State::PlaylistEmpty) == false ? SetState(State::FilesDroppedNotLoaded) : (void)0;
		SetState(State::FilesAddedInfoNotLoaded);
	}
}