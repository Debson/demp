#include "music_player.h"

#include "music_player_system.h"
#include "music_player_state.h"

namespace mdEngine
{
	namespace MP
	{
		
	}

	MP::MusicPlayer::~MusicPlayer() 
	{ 
		
	}

	void MP::MusicPlayer::Start()
	{
		Data::InitializeData();
		OpenMusicPlayer();
	}

	void MP::MusicPlayer::Update()
	{
		Data::UpdateData();
		UpdateInput();
		UpdateLogic();
		State::ResetStateFlags();
	}

	void MP::MusicPlayer::Render()
	{
		RenderMusicPlayer();
	}

	void MP::MusicPlayer::Close()
	{
		MP::CloseMusicPlayer();
		Data::CloseData();
	}
	
}