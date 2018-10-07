#include "music_player_config.h"

#include "../audio/mp_audio.h"
#include "../utility/md_parser.h"
#include "../app/application_window.h"
#include "../playlist/music_player_playlist.h"
#include "../graphics/music_player_graphics.h"
#include "../settings/music_player_string.h"


namespace mdEngine
{
	namespace MP
	{
		namespace Config
		{

		}

		void Config::LoadConfig()
		{
			// Initialize config path before accessing it
			Strings::InitializeStrings();

			App::InitializeConfig();
			Graphics::InitializeConfig();
			Playlist::InitializeConfig();
			Audio::InitializeConfig();
			Parser::ReadPathsFromFile(Strings::_PLAYLIST_FILE);
		}

		void Config::SaveToConfig()
		{
			md_log(Audio::Object::GetSize());
			Parser::SavePathsToFile(Strings::_PLAYLIST_FILE);
			Parser::SaveSettingsToFile(Strings::_SETTINGS_FILE);
		}

	}
}