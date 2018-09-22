#include "md_parser.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <iostream>
#include <Windows.h>
#include <locale>
#include <codecvt>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "md_util.h"
#include "utf8_to_utf16.h"
#include "../app/realtime_system_application.h"
#include "../audio/mp_audio.h"
#include "../settings/music_player_string.h"
#include "../interface/md_interface.h"
#include "../graphics/music_player_graphics_playlist.h"
#include "../playlist/music_player_playlist.h"
#include "../player/music_player_system.h"
#include "../player/music_player_state.h"

// title, artist, album, genre, year, track num, composer, bitrate, channels, freq, size, length,   
#define SEPARATOR '|'
#define SEPARATOR_W L'|'
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
		T GetValueAtPos(std::wstring path, s32 pos);

		std::wstring GetStringAtPos(std::wstring path, s32 pos);

		b8 is_number(const std::string& s);

		size_t GetSizeOfFile(const std::wstring& path)
		{
			struct _stat fileinfo;
			_wstat(path.c_str(), &fileinfo);
			return fileinfo.st_size;
		}

		std::wstring LoadUtf8FileToString(const std::wstring& filename)
		{
			std::wstring buffer;            // stores file contents
			FILE* f = _wfopen(filename.c_str(), L"rtS, ccs=UTF-8");

			// Failed to open file
			if (f == NULL)
			{
				// ...handle some error...
				return buffer;
			}

			size_t filesize = GetSizeOfFile(filename);

			// Read entire file contents in to memory
			if (filesize > 0)
			{
				buffer.resize(filesize);
				size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, f);
				buffer.resize(wchars_read);
				buffer.shrink_to_fit();
			}

			fclose(f);

			return buffer;
		}
	}

	b8 Parser::is_number(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
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

		/* Save All playlist information in specific format to a file
		*/

		file << "#-----SUMMARY-----#\n";
		AddToFile(&file, Strings::_CONTENT_DURATION, std::to_string(Graphics::MP::GetPlaylistObject()->GetItemsDuration()));
		AddToFile(&file, Strings::_CONTENT_FILES, Audio::Object::GetSize());
		AddToFile(&file, Strings::_CONTENT_SIZE, std::to_string(Graphics::MP::GetPlaylistObject()->GetItemsSize()));
		AddToFile(&file, Strings::_CONTENT_LOADED, Audio::Info::GetProcessedItemsCount() == Audio::Object::GetSize());
		AddToFile(&file, Strings::_PLAYBACK_CURSOR, MP::Playlist::RamLoadedMusic.m_ID);

		file << "\n\n";
		file << SEPARATOR_CONTENT;
		file << "\n";

		//std::wstringstream buffer;
		auto sepCon = Interface::Separator::GetContainer();
		for (auto & i : *Interface::Separator::GetContainer())
		{
			file << "-" << utf16_to_utf8(i.second->GetSeparatorPath()) << "\n";
			for (auto k : *i.second->GetSubFilesContainer())
			{
				// title, artist, album, genre, year, track num, composer, bitrate, channels, freq, size, length,  
				file << utf16_to_utf8(k.second);
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
				file << "\n";

				//buffer << k.second;
				//buffer << std::endl;
			}
		}

		file.close();

		return true;
	}

	b8 Parser::ReadPathsFromFile(const std::string& fileName)
	{
		// Move to line with content(paths and file info)
		s32 filesInfoScanned = Parser::GetInt(fileName, Strings::_CONTENT_LOADED);
		s32 filesDuration = Parser::GetInt(fileName, Strings::_CONTENT_DURATION);
		if (filesDuration <= 0 && filesInfoScanned > 0)
		{
			filesInfoScanned = 0;
		}

		f32 start = SDL_GetTicks();
		std::wstring input = LoadUtf8FileToString(utf8_to_utf16(Strings::_PLAYLIST_FILE));
		std::wstringstream buffer(input);
		input = std::wstring();

		while(getline(buffer, input) && input.compare(L"#-----CONTENT-----#") != 0) { }

		while(getline(buffer, input))
		{
			[&] 
			{
				if (input[0] == '-')
				{
					if (Audio::Folders::AddFolder(input.substr(1, input.length())) == false)
					{
						while (getline(buffer, input) && input[0] != '-') { }
						return;
					}
				}
				else
				{
					// title, artist, album, genre, year, track num, composer, bitrate, channels, freq, size, length,  

					/* Read path and file properties from file, save it to ID3 structure and send it to the
					   function that will process sent informations
					*/
					State::SetState(State::PathLoadedFromFileVolatile);
					State::SetState(State::PathLoadedFromFile);

					s32 pos = input.find_first_of(L'|');
					std::wstring path = input.substr(0, pos);
					Audio::Info::ID3 info;
					// optimize this, somehow
					if (filesInfoScanned == 1)
					{
						info.title = GetStringAtPos(input.substr(pos, input.length()), POSITION_TITLE);
						info.artist = GetStringAtPos(input.substr(pos, input.length()), POSITION_ARTIST);
						info.album = GetStringAtPos(input.substr(pos, input.length()), POSITION_ALBUM);
						info.genre = GetStringAtPos(input.substr(pos, input.length()), POSITION_GENRE);
						info.year = GetStringAtPos(input.substr(pos, input.length()), POSITION_YEAR);
						info.track_num = GetStringAtPos(input.substr(pos, input.length()), POSITION_TRACK_NUM);
						info.composer = GetStringAtPos(input.substr(pos, input.length()), POSITION_COMPOSER);
						info.bitrate = GetValueAtPos<f32>(input.substr(pos, input.length()), POSITION_BITRATE);
						info.channels = GetValueAtPos<s16>(input.substr(pos, input.length()), POSITION_CHANNELS);
						info.freq = GetValueAtPos<f32>(input.substr(pos, input.length()), POSITION_FREQUENCY);
						info.size = GetValueAtPos<f32>(input.substr(pos, input.length()), POSITION_SIZE);
						info.length = GetValueAtPos<f64>(input.substr(pos, input.length()), POSITION_LENGTH);
					}

					Audio::LoadPathsFromFile(path, info);
				}
			}();
		}

		
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

		AddToFile(&file, Strings::_VOLUME, MP::Data::VolumeLevel);;
		AddToFile(&file, Strings::_CURRENT_SONG, utf16_to_utf8(MP::Playlist::RamLoadedMusic.m_Path));
		AddToFile(&file, Strings::_CURRENT_SONG_ID, MP::Playlist::RamLoadedMusic.m_ID);
		AddToFile(&file, Strings::_SONG_POSITION, MP::Playlist::GetPosition());
		AddToFile(&file, Strings::_SHUFFLE_STATE, MP::Playlist::IsShuffleEnabled());
		AddToFile(&file, Strings::_REPEAT_STATE, MP::Playlist::IsRepeatEnabled());
		AddToFile(&file, Strings::_PLAYLIST_STATE, Graphics::MP::GetPlaylistObject()->IsToggled());
		AddToFile(&file, Strings::_APP_HEIGHT, Window::windowProperties.mApplicationHeight);
		AddToFile(&file, Strings::_VOLUME_SCROLL_STEP, MP::Data::VolumeScrollStep);
		AddToFile(&file, Strings::_PLAYLIST_SCROLL_STEP, MP::Data::PlaylistScrollStep);
		AddToFile(&file, Strings::_MAX_RAM_LOADED_SIZE, MP::Data::_MAX_SIZE_RAM_LOADED);
		AddToFile(&file, Strings::_PAUSE_FADE_TIME, MP::Data::PauseFadeTime);
		AddToFile(&file, Strings::_ON_EXIT_MINIMIZE_TO_TRAY, State::CheckState(State::OnExitMinimizeToTray));

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

			try
			{
				val = std::stoi(t);
			}
			catch (std::invalid_argument) { }
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
	

			try
			{
				val = std::stof(t);
			}
			catch (std::invalid_argument) { }
			

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
	T Parser::GetValueAtPos(std::wstring path, s32 pos)
	{
		s32 count = 0;
		s32 strPos = 0;
		std::wstring p(path);
		std::wstringstream ss;
		while (p.find(SEPARATOR_W) != std::wstring::npos && count < pos)
		{
			p = p.substr(p.find(SEPARATOR_W) + 1, p.length());
			count++;
		}

		T value = 0;

		p = p.substr(0, p.find_first_of(SEPARATOR_W));

		ss << p;
		ss >> value;
		
		return (T)value;
	}
	
}

