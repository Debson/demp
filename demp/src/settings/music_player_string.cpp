#include "music_player_string.h"
#include "configuration.h"
#include "../utility/utf8_to_utf16.h"

#include "boost/filesystem.hpp"
#include "../utility/md_util.h"

#ifdef _WIN32_
#include <windows.h>
#include <lmcons.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif // _WIN32_

namespace fs = boost::filesystem;

namespace mdEngine
{
	namespace Strings
	{
		std::string _SAVED_PATH = "C:\\Users\\michd\\Desktop\\test folder with music";
		std::string _FONT_PATH = "assets\\font\\TimesNewRoman.ttf";
		std::string _FONT_DIGITAL_PATH = "assets\\font\\digital.ttf";


		
		std::string _APPDATA_PATH;
		std::string _TEMP_CHILD_CONSOLE_ARG = std::string();
		std::string _SETTINGS_FILE;
		std::string _PLAYLIST_FILE;
		std::string _CURRENT_DIRECTORY_PATH;
	}

	void Strings::InitializeStrings()
	{
		// Get user appdata directory
		char username[UNLEN + 1];
		DWORD username_len = UNLEN + 1;
		GetUserName(username, &username_len);

		char sysDir[UNLEN + 1];
		DWORD sysDir_len = UNLEN + 1;
		GetSystemDirectory(sysDir, sysDir_len);


		_SETTINGS_FILE = sysDir[0];
		_SETTINGS_FILE += ":";
		_SETTINGS_FILE += "\\Users\\";
		_SETTINGS_FILE += username;
		_SETTINGS_FILE += "\\AppData\\Roaming\\demp";

		_APPDATA_PATH = _SETTINGS_FILE + '\\';
		_TEMP_CHILD_CONSOLE_ARG = _APPDATA_PATH + "temp.bin";
		if(fs::exists(utf8_to_utf16(Strings::_TEMP_CHILD_CONSOLE_ARG)))
			fs::remove(utf8_to_utf16(Strings::_TEMP_CHILD_CONSOLE_ARG));


		//C:\Users\debson\AppData\Roaming
		fs::path dir(_SETTINGS_FILE);
		if (fs::create_directory(dir))
		{
			std::cerr << "Directory Created: " << _SETTINGS_FILE << std::endl;
		}

		_SETTINGS_FILE += "\\demp.ini";

		_PLAYLIST_FILE = sysDir[0];
		_PLAYLIST_FILE += ":";
		_PLAYLIST_FILE += "\\Users\\";
		_PLAYLIST_FILE += username;
		_PLAYLIST_FILE += "\\AppData\\Roaming\\demp\\Default.demppl7";
		
		//_SETTINGS_FILE = "E:\\SDL Projects\\demp\\demp\\settings\\settings.txt";
	    //_PATHS_FILE = "E:\\SDL Projects\\demp\\demp\\settings\\path.txt";
	}


	void Strings::SetProgramPath(const char *path)
	{
		_CURRENT_DIRECTORY_PATH = std::string(path);
#ifdef _DEBUG_CURRENT_PATH_
		s32 pos = _CURRENT_DIRECTORY_PATH.find_last_of('\\');
		_CURRENT_DIRECTORY_PATH = _CURRENT_DIRECTORY_PATH.substr(0, pos);
		pos = _CURRENT_DIRECTORY_PATH.find_last_of('\\');
		_CURRENT_DIRECTORY_PATH = _CURRENT_DIRECTORY_PATH.substr(0, pos) + "\\";
#else
		s32 pos = _CURRENT_DIRECTORY_PATH.find_last_of('\\');
		_CURRENT_DIRECTORY_PATH = _CURRENT_DIRECTORY_PATH.substr(0, pos) + "\\";
#endif
		_FONT_PATH = _CURRENT_DIRECTORY_PATH + _FONT_PATH;
		_FONT_DIGITAL_PATH = _CURRENT_DIRECTORY_PATH + _FONT_DIGITAL_PATH;
	}
}