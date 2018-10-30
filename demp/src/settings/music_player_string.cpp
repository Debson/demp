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


namespace mdEngine
{
	namespace Strings
	{
		std::string _SAVED_PATH = "C:\\Users\\michd\\Desktop\\test folder with music";
		std::string _FONT_PATH = "\\assets\\font\\TimesNewRoman.ttf";
		std::string _FONT_DIGITAL_PATH = "\\assets\\font\\digital.ttf";


		std::string _SETTINGS_FILE;
		std::string _PLAYLIST_FILE;
	}

	void Strings::InitializeStrings()
	{
		// Get aboslute path for a font
		char cCurrentPath[FILENAME_MAX];
		GetCurrentDir(cCurrentPath, FILENAME_MAX);
		_FONT_PATH = cCurrentPath + _FONT_PATH;
		_FONT_DIGITAL_PATH = cCurrentPath + _FONT_DIGITAL_PATH;

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
		//C:\Users\debson\AppData\Roaming
		boost::filesystem::path dir(_SETTINGS_FILE);
		if (boost::filesystem::create_directory(dir))
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
}