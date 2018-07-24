#include "music_player.h"
#include "system_music_player.h"

#include <stdio.h>
#include "md_util.h"

namespace mdEngine
{
	Application::SongObject::SongObject()
	{
		mMusic = NULL;
		mPath = NULL;
		mSelected = false;
		id = 0;
	}

	Application::SongObject::~SongObject()
	{
		//Mix_FreeMusic(mMusic);
		mMusic = NULL;
		mPath = NULL;
	}

	b8 Application::SongObject::selected()
	{
		return mSelected;
	}

	void Application::SongObject::select()
	{
		mSelected = true;
	}

	void Application::SongObject::unselect()
	{
		mSelected = false;
	}


	b8 Application::SongObject::load(const char* songPath)
	{
		mPath = songPath;
		mMusic = Mix_LoadMUS(songPath);
		char buffer[128];
		sprintf(buffer, "ERROR: At path: %s", songPath);
		if (mMusic == NULL)
		{
			MD_SDL_ERROR(buffer);
			return false;
		}

		return true;
	}

	Mix_Music* Application::SongObject::get()
	{
		return mMusic;
	}

	/* ***************************************************************************************/

	Application::MusicPlayer::~MusicPlayer() { }

	void Application::MusicPlayer::Start()
	{
		MP::OpenMusicPlayer();
	}

	void Application::MusicPlayer::Update()
	{
		MP::UpdateMusicPlayerInput();
		MP::UpdateMusicPlayerLogic();
	}

	void Application::MusicPlayer::Close()
	{
		
	}

}