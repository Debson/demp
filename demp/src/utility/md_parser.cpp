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
#include "../interface/md_interface.h"
#include "../graphics/music_player_graphics.h"
#include "../playlist/music_player_playlist.h"
#include "../player/music_player_system.h"
#include "../player/music_player_state.h"

// title, artist, album, genre, year, track num, composer, bitrate, channels, freq, size, length,   
#define SEPARATOR '|'
#define SEPARATOR_SUMMARY "#-----SUMMARY-----#" 
#define SEPARATOR_CONTENT "#-----CONTENT-----#"

#define POSITION_TITLE		1
#define POSITION_ARTIST		2
#define POSITION_ALBUM		3
#define POSITION_GENRE		4
#define POSITION_YEAR		5
#define POSITION_TRACK_NUM	6
#define POSITION_COMPOSER	7
#define POSITION_BITRATE	8
#define POSITION_CHANNELS	9
#define POSITION_FREQUENCY	10
#define POSITION_SIZE		11
#define POSITION_LENGTH		12
#define POSITION_FOLDER_REP	13


namespace fs = boost::filesystem;

namespace mdEngine
{
	namespace Parser
	{
		template <typename T>
		std::string NumberToString(T num);
			
		template <typename T>
		b8 AddToFile(std::fstream* file, std::string name, T param);

		template <typename T>
		T GetValueAtPos(std::string path, s32 pos);

		std::wstring GetStringAtPos(std::wstring path, s32 pos);
	}

	b8 Parser::SavePathsToFile(const std::string& fileName)
	{
		std::ofstream file;
		file.open(fileName, std::ios::out | std::ios::binary);
		if (file.is_open() == false)
		{
			MD_ERROR("Error: Could not open a file for writing!\n");
			return false;;
		}

		/* Save All playlist information in specific format to a file
		*/

		file << "#-----SUMMARY-----#\n";
		file << "ContentDuration=" << std::to_string(Graphics::MP::GetPlaylistObject()->GetItemsDuration()) << std::endl;
		file << "ContentFiles=" << Audio::Object::GetSize() << std::endl;
		file << "ContentSize=" << std::to_string(Graphics::MP::GetPlaylistObject()->GetItemsSize()) << std::endl;
		file << "PlaybackCursor=" << MP::Playlist::RamLoadedMusic.mID << std::endl;
		file << "\n\n";
		file << SEPARATOR_CONTENT;
		file << "\n";

		for (auto & i : *Interface::Separator::GetContainer())
		{
			file << "-" << utf16_to_utf8(i.second->GetSeparatorPath()) << "\n";
			for (auto k : *i.second->GetSubFilesContainer())
			{
				//fs::path ps(k.second);
				//if (p.wstring().compare(ps.branch_path().wstring()) == 0)
				{
					// title, artist, album, genre, year, track num, composer, bitrate, channels, freq, size, length,  
					file << utf16_to_utf8(k.second);
					//file << utf16_to_utf8(Audio::Object::GetAudioObject(k.first)->GetPath());
					file << SEPARATOR;
					file << utf16_to_utf8(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().title);
					file << SEPARATOR;
					file << utf16_to_utf8(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().artist);
					file << SEPARATOR;
					file << utf16_to_utf8(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().album);
					file << SEPARATOR;
					file << utf16_to_utf8(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().genre);
					file << SEPARATOR;
					file << utf16_to_utf8(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().year);
					file << SEPARATOR;
					file << utf16_to_utf8(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().track_num);
					file << SEPARATOR;
					file << utf16_to_utf8(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().composer);
					file << SEPARATOR;
					file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().bitrate);
					file << SEPARATOR;
					file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().channels);
					file << SEPARATOR;
					file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().freq);
					file << SEPARATOR;
					file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().size);
					file << SEPARATOR;
					file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct().length);
					file << SEPARATOR;
					file << Audio::Object::GetAudioObject(*k.first)->IsFolderRep();
					file << "\n";

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

		// Move to line with content(paths and file info)
		while(getline(file, input) && input.compare(SEPARATOR_CONTENT) != 0) { }

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
				{
					// title, artist, album, genre, year, track num, composer, bitrate, channels, freq, size, length,  

					/* Read path and file properties from file, save it to ID3 structure and send it to the
					   function that will process sent informations
					*/
					State::PathLoadedFromFile = true;
					s32 pos = input.find_first_of(SEPARATOR);
					std::wstring path = utf8_to_utf16(input.substr(0, pos));
					Audio::Info::ID3 info;

					info.title		= GetStringAtPos(utf8_to_utf16(input.substr(pos, input.length())),		POSITION_TITLE);
					info.artist		= GetStringAtPos(utf8_to_utf16(input.substr(pos, input.length())),		POSITION_ARTIST);
					info.album		= GetStringAtPos(utf8_to_utf16(input.substr(pos, input.length())),		POSITION_ALBUM);
					info.genre		= GetStringAtPos(utf8_to_utf16(input.substr(pos, input.length())),		POSITION_GENRE);
					info.year		= GetStringAtPos(utf8_to_utf16(input.substr(pos, input.length())),		POSITION_YEAR);
					info.track_num	= GetStringAtPos(utf8_to_utf16(input.substr(pos, input.length())),		POSITION_TRACK_NUM);
					info.composer	= GetStringAtPos(utf8_to_utf16(input.substr(pos, input.length())),		POSITION_COMPOSER);
					info.bitrate	= GetValueAtPos<f32>(input.substr(pos, input.length()),					POSITION_BITRATE);
					info.channels	= GetValueAtPos<s16>(input.substr(pos, input.length()),					POSITION_CHANNELS);
					info.freq		= GetValueAtPos<f32>(input.substr(pos, input.length()),					POSITION_FREQUENCY);
					info.size		= GetValueAtPos<f32>(input.substr(pos, input.length()),					POSITION_SIZE);
					info.length		= GetValueAtPos<f64>(input.substr(pos, input.length()),					POSITION_LENGTH);
					info.folder_rep = GetValueAtPos<b8>(input.substr(pos, input.length()),					POSITION_FOLDER_REP);
					
					/*std::cout << utf16_to_utf8(path) << std::endl;
					std::wcout << info.title << std::endl;
					std::wcout << info.artist << std::endl;
					std::wcout << info.album << std::endl;
					std::wcout << info.genre << std::endl;
					std::wcout << info.year << std::endl;
					std::wcout << info.track_num << std::endl;
					std::wcout << info.composer << std::endl;
					std::wcout << std::to_wstring(info.bitrate) << std::endl;
					std::wcout << std::to_wstring(info.channels) << std::endl;
					std::wcout << std::to_wstring(info.freq) << std::endl;
					std::wcout << std::to_wstring(info.size) << std::endl;
					std::wcout << std::to_wstring(info.length) << std::endl << std::endl;*/

					Audio::SavePathFiles(path, info);
				}
			}();
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
		AddToFile(&file, Strings::_PLAYLIST_STATE, Graphics::MP::GetPlaylistObject()->IsToggled());
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

	std::wstring Parser::GetStringAtPos(std::wstring path, s32 pos)
	{
		s32 count = 0;
		s32 strPos = 0;
		std::wstring p(path);
		while (p.find(SEPARATOR) != std::string::npos && count < pos)
		{
			p = p.substr(p.find(SEPARATOR) + 1, p.length());
			count++;
		}


		p = p.substr(0, p.find_first_of(SEPARATOR));
		
		return p;
	}

	template <typename T>
	T Parser::GetValueAtPos(std::string path, s32 pos)
	{
		s32 count = 0;
		s32 strPos = 0;
		std::string p(path);
		std::stringstream ss;
		while (p.find(SEPARATOR) != std::string::npos && count < pos)
		{
			p = p.substr(p.find(SEPARATOR) + 1, p.length());
			count++;
		}

		T value = 0;

		p = p.substr(0, p.find_first_of(SEPARATOR));

		ss << p;
		ss >> value;
		
		return (T)value;
	}
	
}

