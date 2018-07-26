#include "music_player.h"

#include <fstream>
#include <assert.h>

#include "md_util.h"


namespace mdEngine
{
	namespace MP
	{
		MusicPlayer::~MusicPlayer() { }

		void MusicPlayer::Start()
		{
			OpenMusicPlayer();
		}

		void MusicPlayer::Update()
		{
			UpdateMusicPlayerInput();
			UpdateMusicPlayerLogic();
		}

		void MP::MusicPlayer::Close()
		{

		}
	}
}