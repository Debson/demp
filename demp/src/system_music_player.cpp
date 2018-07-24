#include <vector>
#include <algorithm> // std::find
#include <string>

#include "system_music_player.h"
#include "system_application_input.h"
#include "music_player.h"
#include "md_time.h"
#include "music_player_settings.h"

namespace mdEngine
{
namespace MP
{
	std::vector<Application::SongObject> mdCurrentPlaylist;

	b8 mdNextRequest(false);
	b8 mdPreviousRequest(false);
	b8 mdCurrentRequest(false);

	namespace Playlist
	{
		Application::SongObject mdPreviousMusic;
		Application::SongObject mdCurrentMusic;
		Application::SongObject mdNextMusic;

		f32 mdVolume = 50;

		f32 mdVolumeFadeInValue = 0; // shouldnt be here
		f32 mdVolumeFadeOutValue = 0; // shouldnt be here

		b8 mdMusicPaused(false); // shouldnt be here

		b8 mdVolumeFadeIn(false); // shouldnt be here
		b8 mdVolumeFadeOut(false); // shouldnt be here

		void InitializePlaylist()
		{
			mdPreviousMusic = mdCurrentPlaylist[0];
			mdCurrentMusic	= mdCurrentPlaylist[0];
			mdNextMusic		= mdCurrentPlaylist[0];

			mdCurrentMusic.select();
		}

		void UpdatePlaylist()
		{
			
			if (mdNextRequest)
			{
				HaltMusic();

				PlayMusic();

				mdNextRequest = false;
			}

			if (mdPreviousRequest)
			{
				HaltMusic();
				
				PlayMusic();

				mdPreviousRequest = false;
			}
		}

		void UpdateMusic()
		{
			if (mdVolumeFadeOut)
			{
				VolumeFadeOut();
			}
			else if (mdVolumeFadeIn)
			{
				VolumeFadeIn();
			}
			else
			{
				SetMusicVolume(mdVolume);
			}

		}

		void PlayMusic()
		{

			Mix_PlayMusic(mdCurrentMusic.get(), -1);
			
		}

		void HaltMusic()
		{
			Mix_HaltMusic();
		}

		void PauseMusic()
		{
			mdMusicPaused == false ? (mdMusicPaused = true) : (mdMusicPaused = false);

			if (mdMusicPaused)
			{
				mdVolumeFadeOut = true;
				mdVolumeFadeOutValue = mdVolume;
			}
			else
			{
				mdVolumeFadeIn = true;
				mdVolumeFadeInValue = 0;
			}

		}

		void NextMusic()
		{
			if ((mdCurrentMusic.id + 1) < mdCurrentPlaylist.size())
			{
				mdCurrentMusic = mdCurrentPlaylist[mdCurrentMusic.id + 1];
				
				mdNextRequest = true;
			}
			else
			{
				mdCurrentMusic = mdCurrentPlaylist[0];

				mdNextRequest = true;
			}

		}

		void PreviousMusic()
		{
			if ((mdCurrentMusic.id - 1) >= 0)
			{
				mdCurrentMusic = mdCurrentPlaylist[mdCurrentMusic.id - 1];

				mdPreviousRequest = true;
			}
			else
			{
				mdCurrentMusic = mdCurrentPlaylist[mdCurrentPlaylist.size() - 1];

				mdPreviousRequest = true;
			}
		}


		void IncreaseVolume()
		{
			if (mdVolume > 128)
			{
				mdVolume = 128;
			}
			else
			{
				mdVolume += (Settings::VolumeMultiplier * Time::deltaTime);
			}
		}

		void LowerVolume()
		{
			if (mdVolume < 0)
			{
				mdVolume = 0;
			}
			else
			{
				mdVolume -= (Settings::VolumeMultiplier * Time::deltaTime);
			}
		}

		void VolumeFadeIn()
		{
			if (mdVolumeFadeInValue < mdVolume)
			{
				if(Mix_PausedMusic() == 1)
					Mix_ResumeMusic();
				mdVolumeFadeInValue += (Settings::VolumeFade * Time::deltaTime);
				SetMusicVolume(mdVolumeFadeInValue);
			}
			else
			{
				mdVolumeFadeIn = false;
			}
			
		}

		void VolumeFadeOut()
		{
			if (mdVolumeFadeOutValue > 0)
			{
				mdVolumeFadeOutValue -= (Settings::VolumeFade * Time::deltaTime);
				SetMusicVolume(mdVolumeFadeOutValue);
			}
			else
			{
				mdVolumeFadeOut = false;
				Mix_PauseMusic();
			}
		}

		void SetMusicVolume(s8 vol)
		{
			Mix_VolumeMusic(vol);
		}


	}

}

namespace MP
{

	void OpenMusicPlayer(void)
	{
		
	}


	void UpdateMusicPlayerInput(void)
	{

		/*	P = Play
			O = Pause/Unause
			J = Halt
			H = Next
			G = Previous
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
			Playlist::IncreaseVolume();
		}

		if (Application::Input::IsKeyDown(Application::KeyCode::Down))
		{
			Playlist::LowerVolume();
		}



	}

	void UpdateMusicPlayerLogic(void)
	{
		/* Update playlist state */
		Playlist::UpdatePlaylist();

		/* Update the volume */
		Playlist::UpdateMusic();

		/* volume in % */
		std::cout << ((float)Playlist::mdVolume / 128.f) * 100.f << std::endl;

		

	}

	void PushToPlaylist(const char* path)
	{
		b8 exist = false;
	
		/* Search for existing path in */
		for (u32 i = 0; i < mdCurrentPlaylist.size(); i++)
		{
			if (!strcmp(path, mdCurrentPlaylist[i].mPath))
				exist = true;
		}

		if (exist)
		{
			std::cout << path << " already loaded!\n";
		}
		else
		{

			Application::SongObject new_music;
			if (new_music.load(path))
			{
				new_music.id = mdCurrentPlaylist.size();
				mdCurrentPlaylist.push_back(new_music);

				std::cout << "Music at path: \"" << path << "\" loaded successfuly!\n";
			}

			/* After first music is added to the playlist, initialize playlist */
			if (mdCurrentPlaylist.size() <= 1)
			{
				Playlist::InitializePlaylist();
			}
			
		}
		//mdCurrentPlaylist.push_back()
	}

}
}
