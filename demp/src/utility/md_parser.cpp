#include "md_parser.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <Windows.h>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "md_util.h"
#include "utf8_to_utf16.h"
#include "../app/realtime_system_application.h"
#include "../audio/mp_audio.h"
#include "../settings/music_player_string.h"
#include "../graphics/music_player_graphics.h"
#include "../playlist/music_player_playlist.h"
#include "../player/music_player_system.h"
#include "../player/music_player_state.h"


namespace fs = boost::filesystem;

namespace mdEngine
{
	namespace Parser
	{
		

		template <typename T>
		std::string NumberToString(T num);
			
		template <typename T>
		b8 AddToFile(std::fstream* file, std::string name, T param);

	}

	b8 Parser::SavePathsToFile(const std::string& fileName)
	{
		std::fstream file;
		file.open(fileName, std::ios::out | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for writing!\n");
			return false;;
		}

		/* add current song index etc. */


		std::string outtext;
		file << "\n\n\n\n\n\n";
		file << "#-----CONTENT-----#\n";

		for (u32 i = 0; i < Audio::Folders::GetSize(); i++)
		{
			fs::path p(*Audio::Folders::GetAudioFolder(i));
			file << "-" << utf16_to_utf8(p.wstring()) << "\n";
			for (u32 j = 0; j < Audio::Object::GetSize(); j++)
			{
				fs::path ps(Audio::Object::GetAudioObject(j)->GetPath());
				if (p.wstring().compare(ps.branch_path().wstring()) == 0)
				{
					outtext = utf16_to_utf8(Audio::Object::GetAudioObject(j)->GetPath());
					file << outtext << "\n";

					/**/
				}
			}

		}

		
		file.close();

		return true;
	}

	b8 Parser::ReadPathsFromFile(const std::string& fileName)
	{
		std::ifstream file;
		file.open(fileName, std::ios::in | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for reading!\n");
			return false;
		}

		std::string input;

		while(getline(file, input))
		{
			[&] 
			{
				if (input[0] == '-')
				{
					if (Audio::Folders::AddFolder(utf8_to_utf16(input.substr(1, input.length()))) == false)
					{
						while (getline(file, input) && input[0] != '-') { }
						return;
					}
				}
				else
					Audio::PushToPlaylist(utf8_to_utf16(input));;
			}();

			State::IsPlaylistEmpty = false;
			State::PathLoadedFromFile = true;
		}

		file.close();
		
		return true;
	}

	b8 Parser::SaveSettingsToFile(const std::string& fileName)
	{
		std::fstream file;
		file.open(fileName, std::ios::out | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for writing!\n");
			return false;
		}

		AddToFile(&file, Strings::_VOLUME, MP::Playlist::GetVolume());;
		AddToFile(&file, Strings::_CURRENT_SONG, utf16_to_utf8(MP::Playlist::RamLoadedMusic.mPath));
		AddToFile(&file, Strings::_CURRENT_SONG_ID, MP::Playlist::RamLoadedMusic.mID);
		AddToFile(&file, Strings::_SONG_POSITION, MP::Playlist::GetPosition());
		AddToFile(&file, Strings::_SHUFFLE_STATE, MP::Playlist::IsShuffleEnabled());
		AddToFile(&file, Strings::_REPEAT_STATE, MP::Playlist::IsRepeatEnabled());
		AddToFile(&file, Strings::_PLAYLIST_STATE, Graphics::MP::m_Playlist.IsToggled());
		AddToFile(&file, Strings::_APP_HEIGHT, Window::windowProperties.mApplicationHeight);
		AddToFile(&file, "random1", 5.232);
		AddToFile(&file, "random2", "elo");




		return true;
	}

	b8 Parser::ReadSettingsFromFile(const std::string& fileName)
	{
		std::fstream file;
		file.open(fileName, std::ios::in | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for reading!\n");
			return false;
		}


		return true;
	}


	template <typename T>
	std::string Parser::NumberToString(T num)
	{
		std::stringstream ss;
		ss << num;

		return ss.str();
	}

	template <typename T>
	b8 Parser::AddToFile(std::fstream* file, std::string name, T param)
	{
		*file << name << "\t" << "\"" << param << "\"" << std::endl;;

		return true;
	
	}

	std::string Parser::GetString(const std::string& fileName, const std::string& valName)
	{
		std::fstream file;
		file.open(fileName, std::ios::in | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for reading!\n");
			return false;
		}

		std::string t;
		while (getline(file, t) && t.find(valName) != std::string::npos)
		

		std::cout << t << std::endl;

		file.close();

		return t;
	}

	s32 Parser::GetInt(const std::string& fileName, const std::string& valName)
	{
		std::fstream file;
		file.open(fileName, std::ios::in | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for reading!\n");
			return false;
		};

		std::string t;
		while (getline(file, t)) 
		{
			if (t.find(valName) != std::string::npos)
				break;
		}
		s32 val = 0;

		if (!file.eof())
		{
			s32 pos = t.find("\"");

			t = t.substr(t.find("\"") + 1);
			t[t.length() - 1] = '\0';

			std::stringstream ss;
			ss << t;
			ss >> val;
		}
		else
		{
			MD_ERROR("ERROR: Could find string in file!\n");
		}

		file.close();

		return val;
	}

	f32 Parser::GetFloat(const std::string& fileName, const std::string& valName)
	{
		std::fstream file;
		file.open(fileName, std::ios::in | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for reading!\n");
			return false;
		};

		std::string t;
		while (getline(file, t))
		{
			if (t.find(valName) != std::string::npos)
				break;
		}
		f32 val = 0.f;
		if (!file.eof())
		{
			s32 pos = t.find("\"");

			t = t.substr(t.find("\"") + 1);
			t[t.length() - 1] = '\0';

			val = std::stof(t);
		}
		else
		{
			MD_ERROR("ERROR: Could find string in file!\n");
		}

		file.close();

		return val;
	}

	std::wstring Parser::GetStringUTF8(const std::string& fileName, const std::string& valName)
	{
		std::ifstream file;
		file.open(fileName, std::ios::in | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for reading!\n");
			return false;
		}

		std::string intext;

		std::string t = "";
		while (getline(file, t))
		{
			if (t.find(valName) != std::string::npos)
				break;
		}

		if (!file.eof())
		{
			s32 pos = t.find("\"");

			t = t.substr(t.find("\"") + 1);
			t[t.length() - 1] = '\0';


		}
		else
		{
			MD_ERROR("ERROR: Could find string in file!\n");
		}

		file.close();

		return utf8_to_utf16(t);
	}

	
}

