#include "music_player.h"

#include "music_player_system.h"
#include "music_player_settings.h"


namespace mdEngine
{
	namespace MP
	{
		MusicPlayerState musicPlayerState;
	}

	MP::MusicPlayer::~MusicPlayer() 
	{ 
		musicPlayerState = MusicPlayerState::kIdle; 
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
	}

	void MP::MusicPlayer::Render()
	{
		RenderMusicPlayer();
	}

	void MP::MusicPlayer::Close()
	{
		MP::CloseMusicPlayer();
	}
	
}