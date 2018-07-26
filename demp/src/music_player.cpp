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
			Open();

		}

		void MusicPlayer::Update()
		{
			UpdateInput();
			UpdateLogic();
			Render();
		}

		void MP::MusicPlayer::Close()
		{

		}
	}
}