#include "music_player_system.h"

#include <algorithm>
#include <filesystem>

#include "../ui/music_player_ui.h"
#include "../player/music_player.h"
#include "../utility/md_parser.h"
#include "../settings/music_player_string.h"
#include "../audio/mp_audio.h"
#include "../player/music_player_config.h"
#include "../graphics/graphics.h"

#ifdef _WIN32_
#define OUTPUT std::wcout
#else
#define OUTPUT std::cout
#endif

namespace mdEngine
{
namespace MP
{

	void OpenMusicPlayer(void)
	{
		UI::Start();
		Playlist::Start();
		Audio::StartAudio();


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

		
		/*if (App::Input::IsKeyDown(App::KeyCode::Up))
		{
			Playlist::IncreaseVolume(App::InputEvent::kPressedEvent);
		}

		if (App::Input::IsKeyDown(App::KeyCode::Down))
		{
			Playlist::LowerVolume(App::InputEvent::kPressedEvent);
		}

		if (App::Input::IsKeyDown(App::KeyCode::Up))
		{
			Playlist::IncreaseVolume(App::InputEvent::kPressedEvent);
		}

		if (App::Input::IsKeyDown(App::KeyCode::Down))
		{
			Playlist::LowerVolume(App::InputEvent::kPressedEvent);
		}*/

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
		UI::Close();
		Audio::DeallocateAudioItems();
		Interface::CloseInterface();
	}
}
}
