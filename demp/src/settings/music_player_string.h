#ifndef MUSIC_PLAYER_STRING_H
#define MUSIC_PLAYER_STRING_H

#include <string>
#include <iostream>

namespace mdEngine
{
	namespace Strings
	{

		const std::string _SETTINGS_FILE		= "E:\\SDL Projects\\demp\\demp\\settings\\settings.txt";
		const std::string _PATHS_FILE			= "E:\\SDL Projects\\demp\\demp\\settings\\path.txt";
		const std::string _DATABASE_FILE		= "E:\\SDL Projects\\demp\\demp\\data.db";
		extern std::string _SAVED_PATH;
		const std::string _VOLUME				= "volume";
		const std::string _CURRENT_SONG			= "current_song";
		const std::string _CURRENT_SONG_ID		= "current_song_id";
		const std::string _SHUFFLE_STATE		= "shuffle_state";
		const std::string _REPEAT_STATE			= "repeat_state";
		const std::string _SONG_POSITION		= "song_position";
		const std::string _PLAYLIST_STATE		= "playlist_state";
		const std::string _APP_HEIGHT			= "app_height";
		const std::string _VOLUME_SCROLL_STEP	= "volume_scroll_step";
		const std::string _PLAYLIST_SCROLL_STEP = "playlist_scroll_step";
		const std::string _MAX_RAM_LOADED_SIZE  = "max_ram_loaded_size";
		const std::string _PAUSE_FADE_TIME		= "pause_fade_time";


		const std::string _CONTENT_DURATION		= "content_duration";
		const std::string _CONTENT_FILES		= "content_files";
		const std::string _CONTENT_SIZE			= "content_size";
		const std::string _CONTENT_LOADED		= "content_loaded";
		const std::string _PLAYBACK_CURSOR		= "playback_cursor";

		const std::wstring _PLAYLIST_ADD_FILE	= L"Files";
		const std::wstring _PLAYLIST_ADD_FOLDER = L"Folder";

	}
}

#endif // !MUSIC_PLAYER_STRING_H
