#include "music_player_system.h"

#include <vector>
#include <string>
#include <assert.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <filesystem>

#include "md_time.h"
#include "settings.h"
#include "music_player_playlist.h"
#include "music_player_ui.h"
#include "music_player.h"
#include "md_parser.h"
#include "music_player_string.h"

#ifdef _WIN32_
#define OUTPUT std::wcout
#else
#define OUTPUT std::cout
#endif


namespace fs = std::experimental::filesystem::v1;

namespace mdEngine
{
	namespace App
	{
		std::string get_ext(char* path);
	}

namespace MP
{

	void OpenMusicPlayer(void)
	{
		UI::Start();
		Parser::ReadPathsFromFile(Strings::_PATHS_FILE);
		Playlist::Start();

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

		
		/* Volume */
		if (App::Input::IsKeyDown(App::KeyCode::Up))
		{
			Playlist::IncreaseVolume(App::InputEvent::kPressedEvent);
		}

		if (App::Input::IsKeyDown(App::KeyCode::Down))
		{
			Playlist::LowerVolume(App::InputEvent::kPressedEvent);
		}

		/* Volume */
		if (App::Input::IsKeyDown(App::KeyCode::Up))
		{
			Playlist::IncreaseVolume(App::InputEvent::kPressedEvent);
		}

		if (App::Input::IsKeyDown(App::KeyCode::Down))
		{
			Playlist::LowerVolume(App::InputEvent::kPressedEvent);
		}

		/*if (App::Input::IsScrollForwardActive())
		{
			Playlist::IncreaseVolume(App::InputEvent::kScrollEvent);
		}

		if (App::Input::IsScrollBackwardActive())
		{
			Playlist::LowerVolume(App::InputEvent::kScrollEvent);
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

		/* volume in % */
		//std::cout << Playlist::mdVolume * 100.f << std::endl;
	}

	void RenderMusicPlayer()
	{
		UI::Render();
	}

#ifdef _WIN32_
	void PushToPlaylist(std::wstring* path)
#else
	void PushToPlaylist(const char* path)
#endif
	{

		b8 exist = false;
		b8 valid = false;

		fs::path pathToDisplay(*path);
#ifdef _WIN32_
		std::wstring ext;
		/* Extract extension from file. If file doesn't have an extension or
		extracted extension is longer than MAX_EXTENSION_LENGTH then it it
		must be a folder. Open it and for all files in that folder save path
		in memory and push it to playlist. */
		if (pathToDisplay.extension().has_extension() &&
			pathToDisplay.extension().wstring().length() <= MAX_EXTENSION_LENGTH)
		{
			ext = pathToDisplay.extension().wstring();
			std::transform(ext.begin(), ext.end(), ext.begin(), towlower);
		}
#else
		std::string ext;
		/* Extract extension from file. If file doesn't have an extension or
		extracted extension is longer than MAX_EXTENSION_LENGTH then it it
		must be a folder. Open it and for all files in that folder save path
		in memory and push it to playlist. */
		if (pathToDisplay.extension().has_extension() &&
			pathToDisplay.extension().string().length() <= MAX_EXTENSION_LENGTH)
		{
			ext = pathToDisplay.extension().string();
		}

#endif
		else
		{
			for (auto & i : fs::directory_iterator(*path))
			{
#ifdef _WIN32_
				std::wstring* dirPath = new std::wstring(i.path().wstring());
				/*wchar_t * dirPath = new wchar_t[i.path().wstring().length() + 1];
				wcscpy(dirPath, i.path().wstring().c_str());*/
#else
				char * dirPath = new char[i.path().string().length() + 1];
				strcpy(dirPath, i.path().string().c_str());
#endif
				PushToPlaylist(dirPath);
			}
		}
	

		/* Check if current path is already in Path Containter. */
		for (u32 i = 0; i < Playlist::mdPathContainer.size(); i++)
		{
#ifdef _WIN32_
			if (path->compare(*Playlist::mdPathContainer[i]) == 0)
#else
			if (strcmp(path, Playlist::mdPathContainer[i]) == 0)
#endif
				exist = true;
		
		}
		/* Check if current's file extension is a valid music format */
		for (u8 i = 0; i < Data::SupportedFormats.size(); i++)
		{
			if (ext.compare(Data::SupportedFormats[i]) == 0)
				valid = true;
		}

		/* Proceed the path */
		if (exist == true)
		{
			OUTPUT << "ERROR: \""<<  *path << "\" already loaded!\n";
			delete path;
			path = NULL;
		}
		else if(valid == false)
		{
			OUTPUT << "ERROR: Invalid extension \"" << ext << "\"!\n";
			delete path;
			path = NULL;
		}
		else if(Playlist::RamLoadedMusic.mMusic == NULL)
		{
			if (Playlist::RamLoadedMusic.init(path))
			{
				OUTPUT << "Music at path: \"" << *path << "\" loaded successfuly!\n";
				std::cout << "Song loaded to the RAM succesfuly\n";
			}
			else
			{
				OUTPUT << "ERROR: " << *path << " cannot be loaded!\n";
				delete path;
				path = NULL;
			}
		}
		else
		{
			/* If song is already loaded to ram, save others's songs paths in vector */
			Playlist::mdPathContainer.push_back(path);
			MP::musicPlayerState = MP::MusicPlayerState::kMusicAdded;
			std::cout << "Song's path saved succesfuly\n";
			UI::PlaylistItem * item = new UI::PlaylistItem();
			item->InitFont();
			item->InitItem();

		}
	}

	void MP::CloseMusicPlayer()
	{
		Parser::SavePathsToFile(Strings::_PATHS_FILE, &Playlist::mdPathContainer);
		Parser::SaveSettingsToFile(Strings::_SETTINGS_FILE);
	}
	/*
	std::string get_ext(char* path)
	{
		char s = '.';
		char *ending = std::strrchr(path, s);
		char buffer[8];

		s8 length = (ending == NULL) ? -1 : strlen(ending);

		if (length > 0 && length < 8)
		{
			for (u8 i = 0; i < strlen(ending); i++)
			{
				buffer[i] = ending[i + 1];
			}
		}
		else
			return path;

		std::string ext = buffer;

		return ext;
	}*/
}
}
