#include "music_player.h"

#include <fstream>
#include <assert.h>

#include "md_util.h"


namespace mdEngine
{
	namespace Application
	{
		b8 check_size(u32);
		b8 check_file();
	}

	b8 Application::check_size(u32 size)
	{
		if (size > _MAX_SIZE_RAM_LOADED)
			return false;
		
		return true;
	}

	b8 Application::check_file()
	{
		if (BASS_ErrorGetCode() == BASS_ERROR_FILEFORM)
		{
			std::cout << "BASS_ERROR: File of this format is not supported!\n";
			return false;
		}
		if (BASS_ErrorGetCode() == BASS_ERROR_FILEOPEN)
		{
			std::cout << "BASS_ERROR: File could not be opened!";
			return false;
		}

		return true;
	}

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

	b8 Application::SongObject::init(const char* songPath)
	{
		std::ifstream file(songPath, std::ios::binary | std::ios::ate);

		if (!file)
		{
			std::cerr << "ERROR::FILE_LOAD: code: " << strerror(errno) 
					  << "\nPath: \"" << songPath << "\"\n!";
			return false;
		}
		else
		{
			std::streamsize size = file.tellg(); // size of opened file
			file.seekg(0, std::ios::beg); // set cursor at the beginning

			mPath = songPath;
			mID = 0;
			mSize = size;

			
			delete[] mData;
			BASS_StreamFree(mMusic);
			mData = NULL;
			mMusic = NULL;
			

			if (check_size(mSize) == true)
			{
				mData = new char[size];
				if (file.read(mData, mSize))
				{
					file.close();
					mMusic = BASS_StreamCreateFile(TRUE, mData, 0, size, BASS_STREAM_AUTOFREE);
					if (check_file() == false)
					{
						return false;
					}
				}
				else
				{
					file.close();
					std::cerr << "ERROR::FILE_READ: code: " << strerror(errno) 
							  << "\nPath: \"" << songPath << "\"\n!";;
					return false;
				}
			}
			else
			{
				delete[] mData;
				mData = NULL;
				mMusic = BASS_StreamCreateFile(FALSE, mPath, 0, 0, 0);

				if (check_file() == false)
				{
					return false;
				}
			}
		}

		return true;

	}

	b8 Application::SongObject::load(const char* songPath, u32 id, MP::Playlist::SongState state)
	{
		std::ifstream file(songPath, std::ios::binary | std::ios::ate);

		if (!file)
		{
			std::cerr << "ERROR::FILE_LOAD: code: " << strerror(errno) << std::endl;
			return false;
		}
		else
		{

			std::streamsize size = file.tellg(); // size of opened file
			file.seekg(0, std::ios::beg); // set cursor at the beginning

			mPath = songPath;
			mID = id;
			mSize = size;


			
				delete[] mData;
				BASS_StreamFree(mMusic);
				mData = NULL;
				mMusic = NULL;
			

			if (check_size(mSize) == true)
			{
				mData = new char[size];
				if (file.read(mData, mSize))
				{
					file.close();
					mMusic = BASS_StreamCreateFile(TRUE, mData, 0, size, BASS_STREAM_AUTOFREE);
					if (check_file() == false)
					{
						delete MP::mdPathContainer[mID];
						MP::mdPathContainer.erase(MP::mdPathContainer.begin() + mID);

						if(state == MP::Playlist::SongState::mNext)
							return load(MP::mdPathContainer[mID - 1], mID - 1, MP::Playlist::SongState::mNext);

						return load(MP::mdPathContainer[mID + 1], mID + 1, MP::Playlist::SongState::mPrevious);
					}
				}
				else
				{
					file.close();
					std::cerr << "ERROR::FILE_READ: code: " << strerror(errno) << std::endl;
					return false;
				}
			}
			else
			{
				delete[] mData;
				mData = NULL;
				mMusic = BASS_StreamCreateFile(FALSE, mPath, 0, 0, 0);

				if (check_file() == false)
				{
					delete MP::mdPathContainer[mID];
					MP::mdPathContainer.erase(MP::mdPathContainer.begin() + mID);

					if (state == MP::Playlist::SongState::mNext)
						return load(MP::mdPathContainer[mID - 1], mID - 1, MP::Playlist::SongState::mNext);

					return load(MP::mdPathContainer[mID + 1], mID + 1, MP::Playlist::SongState::mPrevious);;
				}
			}
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