﻿#include "music_player_system.h"

#include <vector>
#include <string>
#include <assert.h>
#include <cstring>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "md_time.h"
#include "settings.h"
#include "music_player_playlist.h"
#include "music_player_ui.h"

namespace fs = std::experimental::filesystem::v1;

namespace mdEngine
{
	namespace MP
	{
		std::string get_ext(char* path);
	}

namespace MP
{

	void Debug()
	{
		/*
			Kepad7 = list all saved pathes
			Keypad8 = show ram loaded song's id
			Keypad9 = is ram loaded song stream property NULL?
		*/
		if (Input::IsKeyPressed(MP::KeyCode::Keypad7))
		{
			std::cout << "Currently loaded paths:\n";
			for (u32 i = 0; i < Playlist::mdPathContainer.size(); i++)
			{
				std::wcout << Playlist::mdPathContainer[i] << std::endl;
			}
		}

		if (Input::IsKeyPressed(MP::KeyCode::Keypad8))
		{
			std::cout << "ID: " << Playlist::RamLoadedSong.mID << std::endl;
		}

		if (Input::IsKeyPressed(MP::KeyCode::Keypad9))
		{
			(Playlist::RamLoadedSong.get() == NULL) ? (std::cout << "empty\n") : (std::cout << "not empty\n");
		}
	}


	void Open(void)
	{
		UI::Start();
	}


	void UpdateInput(void)
	{
		Debug();

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


		if (Input::IsKeyPressed(MP::KeyCode::P))
		{
			Playlist::PlayMusic();
		}

		if (Input::IsKeyPressed(MP::KeyCode::O))
		{
			Playlist::PauseMusic();
		}


		if (Input::IsKeyPressed(MP::KeyCode::J))
		{
			Playlist::StopMusic();
		}

		if (Input::IsKeyPressed(MP::KeyCode::H))
		{
			Playlist::NextMusic();
		}

		if (Input::IsKeyPressed(MP::KeyCode::G))
		{
			Playlist::PreviousMusic();
		}

		
		/* Volume */
		if (Input::IsKeyDown(MP::KeyCode::Up))
		{
			Playlist::IncreaseVolume(MP::InputEvent::kPressedEvent);
		}

		if (Input::IsKeyDown(MP::KeyCode::Down))
		{
			Playlist::LowerVolume(MP::InputEvent::kPressedEvent);
		}

		/* Volume */
		if (Input::IsKeyDown(MP::KeyCode::Up))
		{
			Playlist::IncreaseVolume(MP::InputEvent::kPressedEvent);
		}

		if (Input::IsKeyDown(MP::KeyCode::Down))
		{
			Playlist::LowerVolume(MP::InputEvent::kPressedEvent);
		}

		if (Input::IsScrollForwardActive())
		{
			Playlist::IncreaseVolume(MP::InputEvent::kScrollEvent);
		}

		if (Input::IsScrollBackwardActive())
		{
			Playlist::LowerVolume(MP::InputEvent::kScrollEvent);
		}

		/* REWIND */
		if (Input::IsKeyPressed(MP::KeyCode::Left))
		{
			Playlist::RewindMusic(-5);
		}

		if (Input::IsKeyPressed(MP::KeyCode::Right))
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

	void Render()
	{
		UI::Render();
	}


	void PushToPlaylist(std::wstring path)
	{

		b8 exist = false;
		b8 valid = false;

		fs::path pathToDisplay(path);
		
		std::wstring ext;
		
		if (pathToDisplay.extension().has_extension() &&
			pathToDisplay.extension().string().length() <= MAX_PATH_LENGTH)
		{
			ext = pathToDisplay.extension().wstring();
		}
		else
		{
			for (auto & i : fs::directory_iterator(path))
			{
				wchar_t * dirPath = new wchar_t[i.path().wstring().length() + 1];
				wcscpy(dirPath, i.path().wstring().c_str());
				PushToPlaylist(dirPath);
			}
		}
	

		/* Search for existing path in */
		for (u32 i = 0; i < Playlist::mdPathContainer.size(); i++)
		{
			if (path.compare(Playlist::mdPathContainer[i]) == 0)
				exist = true;
		}

		for (u8 i = 0; i < Data::SupportedFormats.size(); i++)
		{
			if (ext.compare(Data::SupportedFormats[i]) == 0)
				valid = true;
		}


		if (exist == true)
		{
			std::wcout << "ERROR: \""<<  path << "\" already loaded!\n";
		}
		else if(valid == false)
		{
			std::wcout << "ERROR: Invalid extension \"" << ext << "\"!\n";
		}
		else if(Playlist::RamLoadedSong.mMusic == NULL)
		{
			if (Playlist::RamLoadedSong.init(path))
			{
				Playlist::mdPathContainer.push_back(path);

				std::wcout << "Music at path: \"" << path << "\" loaded successfuly!\n";
				std::wcout << "Song loaded to the RAM succesfuly\n";
			}
			else
			{
				std::wcout << "ERROR: " << path << " cannot be loaded!\n";
			}
		}
		else
		{
			/* If song is already loaded to ram, save others's songs paths in vector */
			Playlist::mdPathContainer.push_back(path);
			std::cout << "Song's path saved succesfuly\n";
		}
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
