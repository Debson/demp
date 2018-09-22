#ifndef MUSIC_PLAYER_STRING_H
#define MUSIC_PLAYER_STRING_H

#include <string>
#include <iostream>

namespace mdEngine
{
	namespace Strings
	{

		extern std::string _SETTINGS_FILE;
		extern std::string _PLAYLIST_FILE;
		const std::string _DATABASE_FILE			= "E:\\SDL Projects\\demp\\demp\\data.db";
		extern std::string _SAVED_PATH;
		const std::string _VOLUME					= "volume";
		const std::string _CURRENT_SONG				= "current_song";
		const std::string _CURRENT_SONG_ID			= "current_song_id";
		const std::string _SHUFFLE_STATE			= "shuffle_state";
		const std::string _REPEAT_STATE				= "repeat_state";
		const std::string _SONG_POSITION			= "song_position";
		const std::string _PLAYLIST_STATE			= "playlist_state";
		const std::string _APP_HEIGHT				= "app_height";
		const std::string _VOLUME_SCROLL_STEP		= "volume_scroll_step";
		const std::string _PLAYLIST_SCROLL_STEP		= "playlist_scroll_step";
		const std::string _MAX_RAM_LOADED_SIZE		= "max_ram_loaded_size";
		const std::string _PAUSE_FADE_TIME			= "pause_fade_time";
		const std::string _ON_EXIT_MINIMIZE_TO_TRAY = "on_exit_minimize_to_tray";


		const std::string _CONTENT_DURATION		= "content_duration";
		const std::string _CONTENT_FILES		= "content_files";
		const std::string _CONTENT_SIZE			= "content_size";
		const std::string _CONTENT_LOADED		= "content_loaded";
		const std::string _PLAYBACK_CURSOR		= "playback_cursor";

		const std::string  _OPTIONS_WINDOW_NAME				= "Options";
		const std::wstring _VOLUME_SCROL_STEP_TEXT			= L"Volume Scroll Step:";
		const std::wstring _PLAYLIST_SCROLL_STEP_TEXT		= L"Playlist Scroll Step:";
		const std::wstring _PAUSE_FADE_TEXT					= L"Volume Fade on Pause/Play Length:";
		const std::wstring _MAX_RAM_LOADED_SIZE_TEXT		= L"Maximum Ram Loaded Size:";
		const std::wstring _ON_EXIT_MINMIZE_TO_TRAY_TEXT	= L"On Exit Minimize To Tray:";

		const std::wstring _PLAYLIST_ADD_FILE	= L"Files";
		const std::wstring _PLAYLIST_ADD_FOLDER = L"Folder";

		void InitializeStrings();
	}
}

#endif // !MUSIC_PLAYER_STRING_H
