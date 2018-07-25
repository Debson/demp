#include "system_music_player.h"

#include <fstream>
#include "md_util.h"

namespace mdEngine
{
	Application::SongObject::SongObject()
	{
		mMusic = NULL;
		mPath = NULL;
	}

	Application::SongObject::~SongObject()
	{
		mMusic = NULL;
		mPath = NULL;
	}

	b8 Application::SongObject::load(const char* songPath, u32 id)
	{
		std::ifstream file(songPath, std::ios::binary | std::ios::ate);

		if (!file)
		{
			std::cerr << "ERROR: file " << songPath << " could not be opened!\n";
			std::cerr << "Errr code: " << strerror(errno);
			return false;
		}
		else
		{
			mPath = songPath;
			mID = id;
			std::streamsize size = file.tellg(); // size of opened file
			mSize = size;
			file.seekg(0, std::ios::beg); // set cursor at the beginning

			if(mData != NULL)
				delete[] mData;

			if(mMusic != NULL)
				BASS_StreamFree(mMusic);

			mData = new char[size]; // 
			if (file.read(mData, size))
			{
				mMusic = BASS_StreamCreateFile(TRUE, mData, 0, size, BASS_STREAM_AUTOFREE);
			}
			else
			{
				std::cerr << "ERROR: file could not be copied into buffer!\n";
				std::cerr << "Errr code: " << strerror(errno);
				return false;
			}

			file.close();
		}

		return true;
	}

	b8 Application::SongObject::update(const char* songPath)
	{

		return true;
	}

	HMUSIC& Application::SongObject::get()
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