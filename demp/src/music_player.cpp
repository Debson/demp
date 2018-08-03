#include "music_player.h"

#include <fstream>
#include <assert.h>

#include "md_util.h"

#include "music_player_system.h"

namespace mdEngine
{
	namespace MP
	{
		MusicPlayerState musicPlayerState;
	}

	MP::MusicPlayer::~MusicPlayer() 
	{ 
		musicPlayerState = MusicPlayerState::kStatic; 
	}

	void MP::MusicPlayer::Start()
	{
		Open();
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

	}
	
}