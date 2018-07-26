#include <vector>
#include <string>
#include <assert.h>
#include <cstring>
#include <sstream>

#include <iostream>

#include <filesystem>
#include <fcntl.h>
#include <io.h>

#include "music_player.h"
#include "md_time.h"
#include "settings.h"

namespace fs = std::experimental::filesystem::v1;

namespace mdEngine
{
namespace MP
{
	namespace Playlist
	{

		Application::SongObject mdPreviousMusic;
		Application::SongObject mdCurrentMusic;
		Application::SongObject mdNextMusic;

		f64 mdVolume = 0.5;

		f64 mdVolumeFadeInValue = 0;
		f64 mdVolumeFadeOutValue = 0;

		b8 mdVolumeFadeIn(false);
		b8 mdVolumeFadeOut(false);

		b8 mdMusicPaused(false);


		void InitializePlaylist();

		void UpdatePlaylist();

		void UpdateMusic();

		void SetMusicVolume(f64 vol);
	}

	std::string get_ext(char* path);
}

namespace MP
{
	struct PathObject
	{
		PathObject(char* path)
		{
			char s = '.';
			char *ending = std::strrchr(path, s);
			char buffer[8];
			for (u8 i = 0; i < strlen(ending); i++)
			{
				buffer[i] = ending[i + 1];
			}
			ext = buffer;
		}

		std::string ext;
	};


	Application::SongObject RamLoadedSong;
	PathContainer mdPathContainer;

	b8 mdNextRequest(false);
	b8 mdPreviousRequest(false);
	b8 mdCurrentRequest(false);

	void Playlist::InitializePlaylist()
	{
		mdPreviousMusic = RamLoadedSong;
		RamLoadedSong = RamLoadedSong;
		mdNextMusic = RamLoadedSong;
	}

	void Playlist::UpdatePlaylist()
	{

		if (mdNextRequest)
		{
			PauseMusic();

			PlayMusic();

			mdNextRequest = false;
		}

		if (mdPreviousRequest)
		{
			PauseMusic();

			PlayMusic();

			mdPreviousRequest = false;
		}
	}

	void Playlist::UpdateMusic()
	{
		float vol = 0;
		BASS_ChannelGetAttribute(RamLoadedSong.get(), BASS_ATTRIB_VOL, &vol);

		if (mdVolumeFadeOut)
		{
			if (vol <= 0)
			{
				BASS_ChannelPause(RamLoadedSong.get());
			}
		}
		else if (mdVolumeFadeIn)
		{
			if (vol >= mdVolume)
			{
				mdVolumeFadeIn = false;
			}
		}
		else
		{
			SetMusicVolume(mdVolume);
		}
		
		if (Application::Input::IsKeyPressed(Application::KeyCode::I))
		{
			std::cout << BASS_ChannelGetLength(RamLoadedSong.get(), BASS_POS_BYTE) << std::endl;
		}

		//std::cout << (BASS_ChannelIsActive(RamLoadedSong.get()) == (BASS_ACTIVE_STOPPED)) << std::endl;
	}

	void Playlist::PlayMusic()
	{
		/* Reset booleans that control music state */
		mdVolumeFadeIn = false;
		mdVolumeFadeOut = false;
		mdMusicPaused = false;

		if (RamLoadedSong.get() != NULL)
		{
			BASS_ChannelPlay(RamLoadedSong.get(), true);
		}

	}

	void Playlist::StopMusic()
	{
		if (RamLoadedSong.get() != NULL)
		{
			BASS_ChannelPause(RamLoadedSong.get());
			BASS_ChannelSetPosition(RamLoadedSong.get(), 0, BASS_POS_BYTE);
		}
	}

	void Playlist::PauseMusic()
	{

		if (BASS_ChannelIsActive(RamLoadedSong.get()) != BASS_ACTIVE_STOPPED && RamLoadedSong.get() != NULL)
		{
			mdMusicPaused == false ? (mdMusicPaused = true) : (mdMusicPaused = false);

			if (mdMusicPaused)
			{
				BASS_ChannelSlideAttribute(RamLoadedSong.get(), BASS_ATTRIB_VOL, 0, Settings::VolumeFadeTime);
				mdVolumeFadeOut = true;
			}
			else
			{
				mdVolumeFadeOut = false;
				BASS_ChannelPlay(RamLoadedSong.get(), false);
				BASS_ChannelSlideAttribute(RamLoadedSong.get(), BASS_ATTRIB_VOL, mdVolume, Settings::VolumeFadeTime);
				mdVolumeFadeIn = true;
			}
		}

	}

	void Playlist::NextMusic()
	{
		if ((RamLoadedSong.get() != NULL) || (BASS_ErrorGetCode() == BASS_ERROR_FILEFORM))
		{
			BASS_ChannelStop(RamLoadedSong.get());

			/*	If song path exist on next position, load it to the ram,
				else load song from path at position 0 in path's container.

				While loop:
					While path in PathContainer at position (current_path_id + index) is not valid and
					path on next position exist, try to open file at that path position.

			*/
			std::cout << RamLoadedSong.mID << std::endl;
			if ((RamLoadedSong.mID + 1) < mdPathContainer.size())
			{
				int index = 1;
				while ((RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID + index], RamLoadedSong.mID + index, SongState::mNext) == false)
					&& (RamLoadedSong.mID + 1 < mdPathContainer.size()))
				{
					index++;
				}
				//assert(RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID + index], RamLoadedSong.mID + index));

				mdNextRequest = true;
			}
			else
			{
				RamLoadedSong.load(mdPathContainer[0], 0, SongState::mCurrent);
				mdNextRequest = true;
			}
		}
	}

	void Playlist::PreviousMusic()
	{
		if (RamLoadedSong.get() != NULL)
		{
			BASS_ChannelStop(RamLoadedSong.get());

			/*	If song path exist on previous position, load it to the ram,
				else load song from path at position `PathContainer.Size() - 1`

				While loop:
					While path in PathContainer at position (current_path_id - index) is not valid and
					path on previous position exist, try to open file at that path position

			*/
			std::cout << RamLoadedSong.mID << std::endl;
			if ((RamLoadedSong.mID - 1) >= 0)
			{
				int index = 1;
				while ((RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID - index], RamLoadedSong.mID - index, SongState::mPrevious) == false)
					&& (RamLoadedSong.mID - index >= 0))
				{
					index++;
				}

				//assert(RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID - index], RamLoadedSong.mID - index));
				mdPreviousRequest = true;
			}
			else
			{
				RamLoadedSong.load(mdPathContainer[mdPathContainer.size() - 1], mdPathContainer.size() - 1, SongState::mCurrent);
				mdPreviousRequest = true;
			}
		}
	}


	void Playlist::IncreaseVolume(Application::InputEvent event)
	{
		if (mdVolume > 1.0)
		{
			mdVolume = 1.0;
		}
		else
		{
			switch (event)
			{
			case Application::InputEvent::kPressedEvent:
				mdVolume += (Settings::VolumeKeyMultiplier * Time::deltaTime);
				break;
			case Application::InputEvent::kScrollEvent:
				mdVolume += (Settings::VolumeScrollStep / 100.f);
				break;
			}
		}

		std::cout << "Volume: " << mdVolume << std::endl;
	}

	void Playlist::LowerVolume(Application::InputEvent event)
	{
		if (mdVolume < 0)
		{
			mdVolume = 0;
		}
		else
		{
			switch (event)
			{
			case Application::InputEvent::kPressedEvent:
				mdVolume -= (Settings::VolumeKeyMultiplier * Time::deltaTime);
				break;
			case Application::InputEvent::kScrollEvent:
				mdVolume -= (Settings::VolumeScrollStep / 100.f);
				break;
			};
		}

		std::cout << "Volume: " << mdVolume << std::endl;
	}

	void Playlist::SetMusicVolume(f64 vol)
	{
		BASS_ChannelSetAttribute(RamLoadedSong.get(), BASS_ATTRIB_VOL, vol);
	}

	void Playlist::RewindMusic(s32 pos)
	{
		if (RamLoadedSong.get() != NULL)
		{
			u64 currentPos = BASS_ChannelGetPosition(RamLoadedSong.get(), BASS_POS_BYTE);
			u64 previousPos = currentPos;
			u64 bytes;
			if (pos < 0)
			{
				bytes = BASS_ChannelSeconds2Bytes(RamLoadedSong.get(), -(pos));
				currentPos -= bytes;
				if (previousPos < currentPos)
					currentPos = 0;
			}
			else
			{
				u64 length = BASS_ChannelGetLength(RamLoadedSong.get(), BASS_POS_BYTE);
				bytes = BASS_ChannelSeconds2Bytes(RamLoadedSong.get(), pos);
				currentPos += bytes;
				if (previousPos >= length)
				{
					PlayMusic();
					currentPos = 0;
				}
			}

			std::cout << "Music pos: " << BASS_ChannelBytes2Seconds(RamLoadedSong.get(), currentPos) << std::endl;
			BASS_ChannelSetPosition(RamLoadedSong.get(), currentPos, BASS_POS_BYTE);
		}
	}

}



namespace MP
{

	void Debug()
	{
		/*
			Kepad7 = list all saved pathes
			Keypad8 = show ram loaded song's id
			Keypad9 = is ram loaded song stream property NULL?
		*/
		if (Application::Input::IsKeyPressed(Application::KeyCode::Keypad7))
		{
			std::cout << "Currently loaded paths:\n";
			for (u32 i = 0; i < mdPathContainer.size(); i++)
			{
				std::cout << mdPathContainer[i] << std::endl;
			}
		}

		if (Application::Input::IsKeyPressed(Application::KeyCode::Keypad8))
		{
			std::cout << "ID: " << RamLoadedSong.mID << std::endl;
		}

		if (Application::Input::IsKeyPressed(Application::KeyCode::Keypad9))
		{
			(RamLoadedSong.get() == NULL) ? (std::cout << "empty\n") : (std::cout << "not empty\n");
		}
	}


	void OpenMusicPlayer(void)
	{
		
	}


	void UpdateMusicPlayerInput(void)
	{
		Debug();

		/*	Temporary UI
			P	= Play
			O	= Pause/Unause
			J	= Halt
			H	= Next
			G	= Previous
			UP	= increase the volume
			DOWN	= lower the volume
			LEFT	= rewind 5 sec backward
			RIGHT	= rewind 5 sec forward
		*/


		if (Application::Input::IsKeyPressed(Application::KeyCode::P))
		{
			Playlist::PlayMusic();
		}

		if (Application::Input::IsKeyPressed(Application::KeyCode::O))
		{
			Playlist::PauseMusic();
		}


		if (Application::Input::IsKeyPressed(Application::KeyCode::J))
		{
			Playlist::StopMusic();
		}

		if (Application::Input::IsKeyPressed(Application::KeyCode::H))
		{
			Playlist::NextMusic();
		}

		if (Application::Input::IsKeyPressed(Application::KeyCode::G))
		{
			Playlist::PreviousMusic();
		}

		
		/* Volume */
		if (Application::Input::IsKeyDown(Application::KeyCode::Up))
		{
			Playlist::IncreaseVolume(Application::InputEvent::kPressedEvent);
		}

		if (Application::Input::IsKeyDown(Application::KeyCode::Down))
		{
			Playlist::LowerVolume(Application::InputEvent::kPressedEvent);
		}

		/* Volume */
		if (Application::Input::IsKeyDown(Application::KeyCode::Up))
		{
			Playlist::IncreaseVolume(Application::InputEvent::kPressedEvent);
		}

		if (Application::Input::IsKeyDown(Application::KeyCode::Down))
		{
			Playlist::LowerVolume(Application::InputEvent::kPressedEvent);
		}

		if (Application::Input::IsScrollForwardActive())
		{
			Playlist::IncreaseVolume(Application::InputEvent::kScrollEvent);
		}

		if (Application::Input::IsScrollBackwardActive())
		{
			Playlist::LowerVolume(Application::InputEvent::kScrollEvent);
		}

		/* REWIND */
		if (Application::Input::IsKeyPressed(Application::KeyCode::Left))
		{
			Playlist::RewindMusic(-5);
		}

		if (Application::Input::IsKeyPressed(Application::KeyCode::Right))
		{
			Playlist::RewindMusic(5);
		}

	}




	void UpdateMusicPlayerLogic(void)
	{
		/* Update playlist state */
		Playlist::UpdatePlaylist();

		/* Update the volume */
		Playlist::UpdateMusic();

		/* volume in % */
		//std::cout << Playlist::mdVolume * 100.f << std::endl;
	}


	void PushToPlaylist(const char* path)
	{
		std::string str = path;
		char *updatedPath = new char[str.length() + 1];
		strcpy(updatedPath, str.c_str());

		b8 exist = false;
		b8 valid = false;

		fs::path pathToDisplay(path);
		
		std::string ext;

		static int count = 0;
		
		if (pathToDisplay.extension().has_extension() &&
			pathToDisplay.extension().string().length() <= MAX_PATH_LENGTH)
		{
			ext = pathToDisplay.extension().string();
			count++;
		}
		else
		{
			for (auto & i : fs::directory_iterator(path))
			{
				char * dirPath = new char[i.path().string().length() + 1];
				strcpy(dirPath, i.path().string().c_str());
				PushToPlaylist(dirPath);
			}
		}
	
		/* Search for existing path in */
		for (u32 i = 0; i < mdPathContainer.size(); i++)
		{
			if (strcmp(path, mdPathContainer[i]) == 0)
				exist = true;
		}

		for (u8 i = 0; i < Data::SupportedFormats.size(); i++)
		{
			if (ext.compare(Data::SupportedFormats[i]) == 0)
				valid = true;
		}


		if (exist == true)
		{
			std::cout << "ERROR: \""<<  path << "\" already loaded!\n";
		}
		else if(valid == false)
		{
			std::cout << "ERROR: Invalid extension \"" << ext << "\"!\n";
		}
		else if(RamLoadedSong.mMusic == NULL)
		{
			if (RamLoadedSong.init(path))
			{
				mdPathContainer.push_back(path);

				std::cout << "Music at path: \"" << path << "\" loaded successfuly!\n";
				std::cout << "Song loaded to the RAM succesfuly\n";
			}
			else
			{
				std::cout << "ERROR: " << path << " cannot be loaded!\n";
			}
		}
		else
		{
			/* If song is already loaded to ram, save others's songs paths in vector */
			mdPathContainer.push_back(path);
			std::cout << "Song's path saved succesfuly\n";
		}
	}

	std::string get_ext(char* path)
	{
		char s = '.';
		char *ending = std::strrchr(path, s);
		char buffer[8];

		s8 length = (ending == NULL) ? -1 : strlen(ending);

		if (length > 0 && length < 8)
		{
			for (u8 i = 0; i < strlen(ending); i++)
			{
				buffer[i] = ending[i + 1];
			}
		}
		else
			return path;

		std::string ext = buffer;

		return ext;
	}
}
}
