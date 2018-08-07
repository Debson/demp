#include "music_player.h"

#include <fstream>
#include <assert.h>

#include "md_util.h"
#include "music_player_system.h""
#include "music_player_playlist.h"
#include "music_player_string.h"


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
		OpenMusicPlayer();
	}

	void MP::MusicPlayer::Update()
	{
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