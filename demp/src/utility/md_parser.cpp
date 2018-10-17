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

#define POSITION_LOADED		1
#define POSITION_TITLE		2
#define POSITION_ARTIST		3
#define POSITION_ALBUM		4
#define POSITION_GENRE		5
#define POSITION_YEAR		6
#define POSITION_TRACK_NUM	7
#define POSITION_COMPOSER	8
#define POSITION_BITRATE	9
#define POSITION_CHANNELS	10
#define POSITION_FREQUENCY	11
#define POSITION_SIZE		12
#define POSITION_LENGTH		13
#define POSITION_CTYPE		14


namespace fs = boost::filesystem;

namespace mdEngine
{
	namespace Parser
	{
		template <typename T>
		std::string NumberToString(T num);
			
		template <typename T>
		b8 AddToFile(std::fstream* file, std::string name, T param);

		void GetFloatAtPos(char* str, f32* info, s32 pos);
		void GetIntAtPos(char* str, s32* info, s32 pos);

		void GetStringAtPos(char* str, std::string& info, s32 pos);

		std::string LoadUtf8FileToString(const std::string& filename)
		{
			std::string buffer;            // stores file contents
			FILE* f = fopen(filename.c_str(), "rtS, ccs=UTF-8");

			// Failed to open file
			if (f == NULL)
			{
				// ...handle some error...
				return buffer;
			}

			fseek(f, 0L, SEEK_END);
			size_t filesize = ftell(f);
			fseek(f, 0L, SEEK_SET);

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

		b8 fileInfoCorrupted(false);
	}

	b8 Parser::SavePathsToFile(const std::string& fileName)
	{
		// Playlist empty, delete playlist settings file
		if (Audio::Object::GetSize() == 0)
		{
			boost::filesystem::remove(fileName);
			return true;
		}

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
		AddToFile(&file, Strings::_CONTENT_LOADED, State::CheckState(State::FilesInfoLoaded) && Audio::Object::GetSize() > 0);
		AddToFile(&file, Strings::_PLAYBACK_CURSOR, MP::Playlist::RamLoadedMusic.m_ID);

		file << "\n\n";
		file << SEPARATOR_CONTENT;
		file << "\n";

		u32 start = SDL_GetTicks();
		//std::stringstream buffer;
		auto sepCon = Interface::Separator::GetContainer();
		for (auto & i : *sepCon)
		{
			file << "-" << i.second->GetSeparatorPath() << "\n";
			for (auto k : *i.second->GetSubFilesContainer())
			{
				// title, artist, album, genre, year, track num, composer, bitrate, channels, freq, size, length,  
				file << *k.second;
				file << SEPARATOR;
				file << Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->loaded;
				file << SEPARATOR;
				file << Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->title;
				file << SEPARATOR;
				file << Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->artist;
				file << SEPARATOR;
				file << Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->album;
				file << SEPARATOR;
				file << Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->genre;
				file << SEPARATOR;
				file << Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->year;
				file << SEPARATOR;
				file << Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->track_num;
				file << SEPARATOR;
				file << Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->composer;
				file << SEPARATOR;
				file << Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->bitrate;
				file << SEPARATOR;
				file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->channels);
				file << SEPARATOR;
				file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->freq);
				file << SEPARATOR;
				file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->size);
				file << SEPARATOR;
				file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->length);
				file << SEPARATOR;
				file << std::to_string(Audio::Object::GetAudioObject(*k.first)->GetID3Struct()->ctype);
				file << "\n";
			}
		}

		md_log(SDL_GetTicks() - start);
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

		FILE* f = fopen(fileName.c_str(), "r");

		if (f == NULL)
		{
			MD_ERROR("file_open");
			return false;
		}

		size_t linesz = 2056;
		char line[2056];
		line[2055] = '\0';
		char otherHalf[2056];
		otherHalf[2055] = '\0';

		size_t len;
		u32 i = 0;

		while ((fgets(line, linesz, f) != NULL) && (strcmp(line, "#-----CONTENT-----#\n") != 0)) { }

		s32 count = 0;
		while (fgets(line, linesz, f) != NULL)
		{
			if (line[0] == '-' || line[1] != ':')
				continue;

			State::SetState(State::InitialLoadFromFile);

			len = strlen(line);
			line[len - 1] = '\0';

			i = 0;
			while (line[i] != '|' && line[i] != '\0')
			{
				i++;
			}

			Audio::Info::ID3* info = new Audio::Info::ID3();
			auto path = new std::string();
			path->resize(i);
			strncat(&path->at(0), line, i);

			// Loaded path could've be corrupted, so check if it is valid before doing any text operations
			if (fs::exists(utf8_to_utf16(*path)) == false)
				continue;

			memcpy(otherHalf, line + i, (len - i));
			memset(line, 0, sizeof(line));

			fileInfoCorrupted = false;
			// title, artist, album, genre, year, track num, composer, bitrate, channels, freq, size, length,  
			s32 loaded = 0;
			GetIntAtPos(otherHalf, &loaded,				POSITION_LOADED);
			info->loaded = loaded;
			GetStringAtPos(otherHalf, info->title,		POSITION_TITLE);
			GetStringAtPos(otherHalf, info->artist,		POSITION_ARTIST);
			GetStringAtPos(otherHalf, info->album,		POSITION_ALBUM);
			GetStringAtPos(otherHalf, info->genre,		POSITION_GENRE);
			GetIntAtPos(otherHalf, &info->year,			POSITION_YEAR);
			GetIntAtPos(otherHalf, &info->track_num,	POSITION_TRACK_NUM);
			GetStringAtPos(otherHalf, info->composer,	POSITION_COMPOSER);
			GetFloatAtPos(otherHalf, &info->bitrate,	POSITION_BITRATE);
			GetIntAtPos(otherHalf, &info->channels,		POSITION_CHANNELS);
			GetFloatAtPos(otherHalf, &info->freq,		POSITION_FREQUENCY);
			GetFloatAtPos(otherHalf, &info->size,		POSITION_SIZE);
			GetFloatAtPos(otherHalf, &info->length,		POSITION_LENGTH);
			s32 ctype = 0;
			GetIntAtPos(otherHalf, &ctype,		POSITION_CTYPE);
			info->ctype = ctype;

			if (fileInfoCorrupted == true)
				info->infoCorrupted = true;

			Audio::LoadPathsFromFile(*path, info);
			count++;
		}

		fclose(f);

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
		AddToFile(&file, Strings::_CURRENT_SONG, MP::Playlist::RamLoadedMusic.m_Path);
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
				val = std::stoi(t, nullptr, 10);
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

	std::string Parser::GetStringUTF8(const std::string& fileName, const std::string& valName)
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

		return t;
	}

	void Parser::GetStringAtPos(char* str, std::string& info, s32 pos)
	{
		s32 count = 0;
		s32 i = 0;
		s32 start_pos = -1;
		s32 end_pos = -1;

		info = "";
		while (str[i] != '\0')
		{
			if (str[i] == SEPARATOR)
				count++;
			if (count == pos && start_pos == -1)
				start_pos = i;
			else if (count == pos + 1)
			{
				end_pos = i - 1;
				break;
			}
			else if (str[i + 1] == '\0')
			{
				end_pos = i;
				break;
			}
			i++;
		}

		if (start_pos == -1 || end_pos == -1)
			return;

		info.resize(end_pos - start_pos);
		memcpy(&info[0], str + start_pos + 1, end_pos - start_pos);

		if (end_pos - start_pos <= 1)
		info = "";
	}

	void Parser::GetFloatAtPos(char* str, f32* info, s32 pos)
	{
		s32 count = 0;
		s32 i = 0;
		s32 start_pos = -1;
		s32 end_pos = -1;

		while (str[i] != L'\0')
		{
			if (str[i] == SEPARATOR)
				count++;
			if (count == pos && start_pos == -1)
				start_pos = i;
			else if (count == pos + 1)
			{
				end_pos = i - 1;
				break;
			}
			else if (str[i + 1] == '\0')
			{
				end_pos = i;
				break;
			}
			i++;
		}

		if (start_pos == -1 || end_pos == -1 || end_pos - start_pos > 30)
		{
			fileInfoCorrupted = true;
			return;
		}

		char strInfo[30];

		memcpy(strInfo, str + start_pos + 1, end_pos - start_pos);
		strInfo[end_pos - start_pos] = '\0';

		*info = atof(strInfo);
	}

	void Parser::GetIntAtPos(char* str, s32* info, s32 pos)
	{
		s32 count = 0;
		s32 i = 0;
		s32 start_pos = -1;
		s32 end_pos = -1;

		while (str[i] != '\0')
		{
			if (str[i] == SEPARATOR)
				count++;
			if (count == pos && start_pos == -1)
				start_pos = i;
			else if (count == pos + 1)
			{
				end_pos = i - 1;
				break;
			}
			else if (str[i + 1] == '\0')
			{
				end_pos = i;
				break;
			}
			i++;
		}

		if (start_pos == -1 || end_pos == -1 || end_pos - start_pos > 30)
			return;

		if (end_pos - start_pos > 30)
		{
			fileInfoCorrupted = true;
			return;
		}

		char strInfo[30];

		memcpy(strInfo, str + start_pos + 1, end_pos - start_pos);
		strInfo[end_pos - start_pos] = '\0';
		std::stringstream ss;
		ss << strInfo;
		ss >> *info;
	}
}

