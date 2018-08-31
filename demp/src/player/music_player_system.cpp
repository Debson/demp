#include "music_player_system.h"

#include <algorithm>
#include <filesystem>

#include "../ui/music_player_ui.h"
#include "../player/music_player.h"
#include "../utility/md_parser.h"
#include "../settings/music_player_string.h"
#include "../audio/mp_audio.h"

#ifdef _WIN32_
#define OUTPUT std::wcout
#else
#define OUTPUT std::cout
#endif


namespace fs = std::experimental::filesystem::v1;

namespace mdEngine
{
	namespace App
	{
		std::string get_ext(char* path);
	}

namespace MP
{

	void OpenMusicPlayer(void)
	{
		UI::Start();
		Parser::ReadPathsFromFile(Strings::_PATHS_FILE);
		Playlist::Start();
		Audio::StartAudio();

		//Database::OpenDB();

		//sqlite::PushToDatabase(path);

	}

	void UpdateInput(void)
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


		if (App::Input::IsKeyPressed(App::KeyCode::P))
		{
			Playlist::PlayMusic();
		}

		if (App::Input::IsKeyPressed(App::KeyCode::O))
		{
			Playlist::PauseMusic();
		}


		if (App::Input::IsKeyPressed(App::KeyCode::J))
		{
			Playlist::StopMusic();
		}

		if (App::Input::IsKeyPressed(App::KeyCode::H))
		{
			Playlist::NextMusic();
		}

		if (App::Input::IsKeyPressed(App::KeyCode::G))
		{
			Playlist::PreviousMusic();
		}

		
		/* Volume */
		if (App::Input::IsKeyDown(App::KeyCode::Up))
		{
			Playlist::IncreaseVolume(App::InputEvent::kPressedEvent);
		}

		if (App::Input::IsKeyDown(App::KeyCode::Down))
		{
			Playlist::LowerVolume(App::InputEvent::kPressedEvent);
		}

		/* Volume */
		if (App::Input::IsKeyDown(App::KeyCode::Up))
		{
			Playlist::IncreaseVolume(App::InputEvent::kPressedEvent);
		}

		if (App::Input::IsKeyDown(App::KeyCode::Down))
		{
			Playlist::LowerVolume(App::InputEvent::kPressedEvent);
		}

		/* REWIND */
		if (App::Input::IsKeyPressed(App::KeyCode::Left))
		{
			Playlist::RewindMusic(-5);
		}

		if (App::Input::IsKeyPressed(App::KeyCode::Right))
		{
			Playlist::RewindMusic(5);
		}
		
	}

	void UpdateLogic(void)
	{
		/* Update playlist state */
		Playlist::UpdatePlaylist();

		/* Update the volume */
		Playlist::UpdateMusic();

		UI::Update();

		Audio::UpdateAudioLogic();

	}

	void RenderMusicPlayer()
	{
		UI::Render();
	}

	void MP::CloseMusicPlayer()
	{
		Parser::SavePathsToFile(Strings::_PATHS_FILE);
		Parser::SaveSettingsToFile(Strings::_SETTINGS_FILE);
		UI::Close();

		//Database::CloseDB();
		Audio::DeallocateAudioItems();
		Interface::CloseInterface();
	}

}
}
