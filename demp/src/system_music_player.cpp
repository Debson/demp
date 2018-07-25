#include <vector>
#include <algorithm> // std::find
#include <string>

#include "system_music_player.h"
#include "music_player.h"
#include "md_time.h"
#include "music_player_settings.h"
#include "md_types.h"

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
}

namespace MP
{
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
			//HaltMusic();

			PlayMusic();

			mdNextRequest = false;
		}

		if (mdPreviousRequest)
		{
			//HaltMusic();

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

	}

	void Playlist::PlayMusic()
	{
		/* Reset booleans that control music state */
		mdVolumeFadeIn = false;
		mdVolumeFadeOut = false;
		mdMusicPaused = false;
		BASS_ChannelPlay(RamLoadedSong.get(), true);
		//Mix_PlayMusic(mdCurrentMusic.get(), -1);

	}

	void Playlist::HaltMusic()
	{
		BASS_ChannelStop(RamLoadedSong.get());
		//Mix_HaltMusic();
	}

	void Playlist::PauseMusic()
	{
		//BASS_ChannelSlideAttribute(mdCurrentMusic.get(), BASS_ATTRIB_VOL, 0, 1000);

		if (BASS_ChannelIsActive(RamLoadedSong.get()) != BASS_ACTIVE_STOPPED)
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
			while ((RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID + index], RamLoadedSong.mID + index) == false)
				&& (RamLoadedSong.mID + 1 < mdPathContainer.size()))
			{
				index++;
			}

			mdNextRequest = true;
		}
		else
		{
			RamLoadedSong.load(mdPathContainer[0], 0);
			mdNextRequest = true;
		}
	}

	void Playlist::PreviousMusic()
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
			while ((RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID - index], RamLoadedSong.mID - index) == false)
				&& (RamLoadedSong.mID - index >= 0))
			{
				index++;
			}

			mdPreviousRequest = true;
		}
		else
		{
			RamLoadedSong.load(mdPathContainer[mdPathContainer.size() - 1], mdPathContainer.size() - 1);
			mdPreviousRequest = true;
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
	}

	void Playlist::SetMusicVolume(f64 vol)
	{
		BASS_ChannelSetAttribute(RamLoadedSong.get(), BASS_ATTRIB_VOL, vol);
	}

	void Playlist::RewindMusic(s32 pos)
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

		std::cout << currentPos << std::endl;
		BASS_ChannelSetPosition(RamLoadedSong.get(), currentPos, BASS_POS_BYTE);
	}

}



namespace MP
{

	void OpenMusicPlayer(void)
	{
		
	}


	void UpdateMusicPlayerInput(void)
	{

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
			Playlist::HaltMusic();
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
		b8 exist = false;
	
		/* Search for existing path in */
		for (u32 i = 0; i < mdPathContainer.size(); i++)
		{
			if (!strcmp(path, mdPathContainer[i]))
				exist = true;
		}

		if (exist)
		{
			std::cout << "ERROR: "<<  path << " already loaded!\n";
		}
		else if(RamLoadedSong.mMusic == NULL)
		{
			/*	Load the currently played song in to the ram .
				Second argument of "load" function is hardcoded 0, 
				because it it initialization call
			*/
			if (RamLoadedSong.load(path, 0))
			{
				mdPathContainer.push_back(path);

				std::cout << "Music at path: \"" << path << "\" loaded successfuly!\n";
			}
			else
			{
				std::cout << "ERROR: " << path << " cannot be loaded!\n";
			}

			std::cout << "Song loaded to the RAM succesfuly\n";
		}
		else
		{
			/* If song is already loaded to ram, save others's songs paths in vector */
			mdPathContainer.push_back(path);
			std::cout << "Song's path saved succesfuly\n";
		}
	}

}
}
